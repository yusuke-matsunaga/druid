#! /usr/bin/env python3
#
# @file dtpg_test.py
# @brief Dtpg のテストプログラム
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.

import argparse
import os.path
import time
from druid_core import FaultType
from druid_core import TpgNetwork
from druid_core import Fsim
from druid_core import TestVector
from druid_core import MinCov
from druid_core import ColCov
from dtpg import Dtpg
from compaction import mincov, coloring
from druid_core import gen_colcov
from druid_core import MinPatMgr

algorithm_list = ('coloring2', 'coloring3', 'mincov+dsatur', 'mincov+isx', 'isx+mincov', 'dsatur+mincov')

def minpat(tv_list, fault_list, network, fault_type, cmp_algorithm) :
    if cmp_algorithm == 'mincov+dsatur' :
        tv_list1 = mincov(fault_list, tv_list, network, fault_type)
        tv_list2 = coloring(tv_list1, 'dsatur')
        return tv_list2
    elif cmp_algorithm == 'mincov+isx' :
        tv_list1 = mincov(fault_list, tv_list, network, fault_type)
        tv_list2 = coloring(tv_list1, 'isx')
        return tv_list2
    elif cmp_algorithm == 'dsatur+mincov' :
        tv_list1 = coloring(tv_list, 'dsatur')
        tv_list2 = mincov(fault_list, tv_list1, network, fault_type)
        return tv_list2
    elif cmp_algorithm == 'isx+mincov' :
        tv_list1 = coloring(tv_list, 'isx')
        tv_list2 = mincov(fault_list, tv_list1, network, fault_type)
        return tv_list2
    elif cmp_algorithm == 'dsatur' :
        tv_list1 = coloring(tv_list, 'dsatur')
        return tv_list1
    elif cmp_algorithm == 'isx' :
        tv_list1 = coloring(tv_list, 'isx')
        return tv_list1
    elif cmp_algorithm == 'coloring2' :
        tv_list1 = MinPatMgr.coloring(fault_list, tv_list, network, fault_type)
        return tv_list1
    elif cmp_algorithm == 'coloring3' :
        fault_list = list(fault_list)
        MinPatMgr.fault_reduction(fault_list, network, fault_type, 'red1,red2')
        tv_list1 = MinPatMgr.coloring(fault_list, tv_list, network, fault_type)
        return tv_list1
    elif cmp_algorithm == 'mincov' :
        tv_list1 = mincov(fault_list, tv_list, network, fault_type)
        return tv_list1
    elif cmp_algorithm != '' :
        print('Error: unknown algorithm "{}"'.format(cmp_algorithm))
        return None


def exec_one(file_name, fault_type, drop) :

    body, ext = os.path.splitext(file_name)
    if ext == '.blif' :
        file_format = 'blif'
    elif ext == '.bench' :
        file_format = 'iscas89'
    else :
        # デフォルト
        file_format = 'blif'

    if file_format == 'blif' :
        network = TpgNetwork.read_blif(file_name)
    elif file_format == 'iscas89' :
        network = TpgNetwork.read_iscas89(file_name)
    else:
        # ここに来るはずはない．
        assert False

    if not network :
        print('Error, could not read {}'.format(file_name))
        return

    start = time.process_time()

    dtpg = Dtpg(network, fault_type)

    ndet, nunt, nabt = dtpg.ffr_mode(drop)

    end = time.process_time()
    cpu_time = end - start

    tf = 0
    for i in network.rep_fault_list() :
        tf += 1
    print('file name:               {}'.format(file_name))
    print('Drop flag:               {}'.format(drop))
    print('# of total faults:       {:8d}'.format(tf))
    print('# of detected faults:    {:8d}'.format(ndet))
    print('# of untestable faults:  {:8d}'.format(nunt))
    print('# of aborted faults:     {:8d}'.format(nabt))
    print('# of initial patterns:   {:8d}'.format(len(dtpg.tvlist)))
    print('CPU time(ATPG):          {:8.2f}'.format(cpu_time))
    for algorithm in algorithm_list :
        start = time.process_time()
        new_tv_list = minpat(dtpg.tvlist, dtpg.fault_list, network, fault_type, algorithm)
        end = time.process_time()
        cpu_time1 = end - start
        nv = len(new_tv_list)
        print('---------------------------------')
        print('Compaction Algorithm:    {}'.format(algorithm))
        print('# of minimized patterns: {:8d}'.format(nv))
        print('CPU time(compaction):    {:8.2f}'.format(cpu_time1))
    print()


def main() :

    parser = argparse.ArgumentParser()

    type_group = parser.add_mutually_exclusive_group()
    type_group.add_argument('--stuck_at',
                            action = 'store_true',
                            help = 'TPG for stuck-at fault [default]')
    type_group.add_argument('--transition_delay',
                            action = 'store_true',
                            help = 'TPG for transition-delay fault')

    parser.add_argument('file_list', metavar = '<filename>', type = str,
                        nargs = '+',
                        help = 'file name')

    args = parser.parse_args()
    if not args :
        exit(1)

    if args.stuck_at :
        fault_type = FaultType.StuckAt
    elif args.transition_delay :
        fault_type = FaultType.TransitionDelay
    else :
        # デフォルト
        fault_type = FaultType.StuckAt

    for drop in (False, True) :
        for file_name in args.file_list :
            exec_one(file_name, fault_type, drop)


if __name__ == '__main__' :
    main()
