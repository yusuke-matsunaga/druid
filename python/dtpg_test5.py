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
from druid_core import MinPatMgr
from dtpg import Dtpg
from exec_one import exec_one

#algorithm_list = ('coloring2', 'mincov+dsatur', 'mincov+isx', 'isx+mincov', 'dsatur+mincov')
algorithm_list = ( 'coloring2', 'mincov+dsatur' )


def main() :

    parser = argparse.ArgumentParser()

    type_group = parser.add_mutually_exclusive_group()
    type_group.add_argument('--stuck_at',
                            action = 'store_true',
                            help = 'TPG for stuck-at fault [default]')
    type_group.add_argument('--transition_delay',
                            action = 'store_true',
                            help = 'TPG for transition-delay fault')

    parser.add_argument('-o', '--output_dir',
                        type = str,
                        help = 'specify the output directory')

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

    odir = args.output_dir
    if not odir :
        odir = '.'
    if os.path.exists(odir) :
        if not os.path.isdir(odir) :
            print('{} is not a directory'.format(odir), file = sys.stderr)
            exit(1)
    else :
        os.mkdir(odir)
        if not os.path.isdir(odir) :
            print('Could not create directory: {}'.format(odir), file = sys.stderr)
            exit(1)

    verbose = args.verbose

    for file_name in args.file_list :
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

        fault_list = MinPatMgr.fault_reduction(dtpg.fault_list, network, fault_type, 'red1,red2')

        end = time.process_time()
        cpu_time = end - start

        if verbose :
            tf = 0
            for i in network.rep_fault_list() :
                tf += 1
            print('file name:               {}'.format(os.path.basename(file_name)))
            print('# of total faults:       {:8d}'.format(tf))
            print('# of detected faults:    {:8d}'.format(ndet))
            print('# of reduced faults:     {:8d}'.format(len(fault_list)))
            print('# of untestable faults:  {:8d}'.format(nunt))
            print('# of aborted faults:     {:8d}'.format(nabt))
            print('CPU time(ATPG):          {:8.2f}'.format(cpu_time))

        result_dic = {}
        for k in ( 1, 2, 5, 10 ) :
            exec_one(dtpg, network, fault_type, fault_list, k, algorithm_list, result_dic, verbose)

        circname, dummy = os.path.splitext(os.path.basename(file_name))
        obasename = '{}.out'.format(circname)
        ofilename = os.path.join(odir, obasename)
        with open(ofilename, 'w') as ofile :
            for sig, ( nv, cpu_time) in result_dic.items() :
                print('{:40}: {:5}: {:8.2f}'.format(sig, nv, cpu_time), file = ofile)

if __name__ == '__main__' :
    main()
