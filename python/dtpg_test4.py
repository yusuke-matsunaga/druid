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
from druid_core import TpgNetwork
from druid_core import FaultType
from druid_core import MinPatMgr
from dtpg import Dtpg
from exec_one import exec_one

#algorithm_list = ('coloring2', 'mincov+dsatur', 'mincov+isx', 'isx+mincov', 'dsatur+mincov')
algorithm_list = ( 'coloring2', 'mincov+dsatur' )

def dtpg1(file_name, fault_type) :
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

    ndet, nunt, nabt = dtpg.ffr_mode(False)

    #fault_list = MinPatMgr.fault_reduction(dtpg.fault_list, network, fault_type, 'red1,red2')
    fault_list = MinPatMgr.fault_reduction(dtpg.fault_list, network, fault_type, 'red2')

    end = time.process_time()
    cpu_time = end - start

    return dtpg, network, fault_list


def main() :

    parser = argparse.ArgumentParser()

    type_group = parser.add_mutually_exclusive_group()
    type_group.add_argument('--stuck_at',
                            action = 'store_true',
                            help = 'TPG for stuck-at fault [default]')
    type_group.add_argument('--transition_delay',
                            action = 'store_true',
                            help = 'TPG for transition-delay fault')
    parser.add_argument('-k',
                       default = 2,
                       help = 'k-detection [default = 2]')

    parser.add_argument('-v', '--verbose',
                        action = 'store_true',
                        help = 'make verbose')

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

    verbose = args.verbose

    k = int(args.k)
    for file_name in args.file_list :
        dtpg, network, fault_list = dtpg1(file_name, fault_type)
        r_dic = {}
        exec_one(dtpg, network, fault_type, fault_list, k, algorithm_list, r_dic, verbose)
        for sig, ( nv, cpu_time) in r_dic.items() :
            print('{:40}: {:5}: {:8.2f}'.format(sig, nv, cpu_time))


if __name__ == '__main__' :
    main()
