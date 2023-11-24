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
from dtpg import Dtpg
from compaction import compaction
#from druid_core import gen_covering_matrix


def gen_covering_matrix(fault_list, tv_list, network, fault_type) :
    nf = len(fault_list)
    fid_dict = {}
    fid = 0
    for fault in fault_list :
        fid_dict[fault.id] = fid
        fid += 1
    assert fid == nf

    mincov = MinCov(nf, len(tv_list))
    #print('MinCov: {} x {}'.format(nf, len(tv_list)))
    fsim = Fsim('Fsim3', network, fault_type)
    tv_buff = []
    wpos = 0
    tid_base = 0
    for tv in tv_list :
        tv_buff.append(tv)
        if len(tv_buff) == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, patid_list in fp_list :
                fid = fid_dict[fault.id]
                for patid in patid_list :
                    #print('  insert({}, {})'.format(fid, tid_base + patid))
                    mincov.insert_elem(fid, tid_base + patid)
            tv_buff = []
            tid_base += 64
    if len(tv_buff) > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for fault, patid_list in fp_list :
            fid = fid_dict[fault.id]
            for patid in patid_list :
                #print('  insert({}, {})'.format(fid, tid_base + patid))
                mincov.insert_elem(fid, tid_base + patid)

    cost, solution = mincov.heuristic()
    print('# of initial patterns: {}'.format(len(tv_list)))
    print('# of reduced patterns: {}'.format(cost))
    return [ tv_list[id] for id in solution ]


def main() :

    parser = argparse.ArgumentParser()

    mode_group = parser.add_mutually_exclusive_group()
    mode_group.add_argument('-f', '--ffr',
                            action = 'store_true',
                            help = 'run in FFR mode [default]')
    mode_group.add_argument('-m', '--mffc',
                            action = 'store_true',
                            help = 'run in MFFC mode')

    type_group = parser.add_mutually_exclusive_group()
    type_group.add_argument('--stuck_at',
                            action = 'store_true',
                            help = 'TPG for stuck-at fault [default]')
    type_group.add_argument('--transition_delay',
                            action = 'store_true',
                            help = 'TPG for transition-delay fault')

    fmt_group = parser.add_mutually_exclusive_group()
    fmt_group.add_argument('--blif',
                            action = 'store_true',
                            help = 'read blif file [default]')
    fmt_group.add_argument('--iscas89',
                           action = 'store_true',
                           help = 'read ISCAS89 file')

    parser.add_argument('--drop',
                        action = 'store_true',
                        help = 'fault drop mode')

    parser.add_argument('--compaction',
                        type = str,
                        metavar = '<compaction algorithm>',
                        help = 'specify compaction algorithm')

    parser.add_argument('file_list', metavar = '<filename>', type = str,
                        nargs = '+',
                        help = 'file name')

    args = parser.parse_args()
    if not args :
        exit(1)

    if args.ffr :
        mode = 'ffr'
    elif args.mffc :
        mode = 'mffc'
    else :
        # デフォルト
        mode = 'ffr'

    drop = args.drop

    if args.stuck_at :
        fault_type = FaultType.StuckAt
    elif args.transition_delay :
        fault_type = FaultType.TransitionDelay
    else :
        # デフォルト
        fault_type = FaultType.StuckAt

    if args.blif :
        file_format = 'blif'
    elif args.iscas89 :
        file_format = 'iscas89'
    else :
        file_format = None

    cmp_algorithm = args.compaction

    for file_name in args.file_list :
        file_format1 = file_format
        if not file_format1 :
            body, ext = os.path.splitext(file_name)
            if ext == '.blif' :
                file_format1 = 'blif'
            elif ext == '.bench' :
                file_format1 = 'iscas89'
            else :
                # デフォルト
                file_format1 = 'blif'

        if file_format1 == 'blif' :
            network = TpgNetwork.read_blif(file_name)
        elif file_format1 == 'iscas89' :
            network = TpgNetwork.read_iscas89(file_name)
        else:
            # ここに来るはずはない．
            assert False

        if not network :
            print('Error, could not read {}'.format(file_name))
            continue

        start = time.process_time()

        dtpg = Dtpg(network, fault_type)

        if mode == 'ffr' :
            ndet, nunt, nabt = dtpg.ffr_mode(drop)
        elif mode == 'mffc' :
            ndet, nunt, nabt = dtpg.mffc_mode(drop)

        lap1 = time.process_time()
        cpu_time = lap1 - start

        tvlist = dtpg.tvlist
        tvlist = gen_covering_matrix(dtpg.fault_list, tvlist, network, fault_type)

        lap2 = time.process_time()
        print('CPU time for covering_matrix: {:8.2f}'.format(lap2 - lap1))
        lap1 = lap2

        if cmp_algorithm :
            tvlist = compaction(tvlist, cmp_algorithm)

        end = time.process_time()
        cpu_time2 = end - lap1

        tf = 0
        for i in network.rep_fault_list() :
            tf += 1
        print('file name:              {}'.format(file_name))
        print('# of total faults:      {:8d}'.format(tf))
        print('# of detected faults:   {:8d}'.format(ndet))
        print('# of untestable faults: {:8d}'.format(nunt))
        print('# of aborted faults:    {:8d}'.format(nabt))
        print('# of patterns:          {:8d}'.format(len(tvlist)))
        print('CPU time(ATPG):         {:8.2f}'.format(cpu_time))
        print('CPU time(compaction):   {:8.2f}'.format(cpu_time2))


if __name__ == '__main__' :
    main()
