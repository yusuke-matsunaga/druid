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
from satpg_core import FaultType
from satpg_core import TpgNetwork
from satpg_core import Fsim
from satpg_core import TestVector
from dtpg import Dtpg
from compaction import compaction
#from satpg_core import gen_covering_matrix


def gen_covering_matrix(tv_list, network, fault_type) :
    fsim = Fsim('Fsim3', network, fault_type)
    tv_buff = [ TestVector() for i in range(64) ]
    wpos = 0
    pat_list_dict = dict()
    for tv in tv_list :
        tv_buff[wpos] = tv
        wpos += 1
        if wpos == 64 :
            fp_list = fsim.ppsfp(tv_buff)
            for fault, patid_list in fp_list :
                if not fault.id in pat_list_dict :
                    pat_list_dict[fault.id] = list()
                for patid in patid_list :
                    pat_list_dict[fault.id].append(tv_buff[patid])
            wpos = 0
    if wpos > 0 :
        fp_list = fsim.ppsfp(tv_buff)
        for fault, patid_list in fp_list :
            if not fault.id in pat_list_dict :
                pat_list_dict[fault.id] = list()
            for patid in patid_list :
                pat_list_dict[fault.id].append(tv_buff[patid])

def main() :

    parser = argparse.ArgumentParser()

    mode_group = parser.add_mutually_exclusive_group()
    mode_group.add_argument('-s', '--single',
                            action = 'store_true',
                            help = 'run in single mode [default]')
    mode_group.add_argument('-f', '--ffr',
                            action = 'store_true',
                            help = 'run in FFR mode')
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

    if args.single :
        mode = 'single'
    elif args.ffr :
        mode = 'ffr'
    elif args.mffc :
        mode = 'mffc'
    else :
        # デフォルト
        mode = 'single'

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

        if mode == 'single' :
            ndet, nunt, nabt = dtpg.single_mode(drop)
        elif mode == 'ffr' :
            ndet, nunt, nabt = dtpg.ffr_mode(drop)
        elif mode == 'mffc' :
            ndet, nunt, nabt = dtpg.mffc_mode(drop)

        lap1 = time.process_time()
        cpu_time = lap1 - start

        gen_covering_matrix(dtpg.tvlist, network, fault_type)

        lap2 = time.process_time()
        print('CPU time for covering_matrix: {:8.2f}'.format(lap2 - lap1))
        lap1 = lap2

        tvlist = dtpg.tvlist
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
