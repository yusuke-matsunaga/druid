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
from druid import TpgNetwork
from druid import FaultType
from druid import MinPatMgr
from druid import Dtpg
from exec_one import exec_one

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

    dtpg = Dtpg(network, fault_type)

    ndet, nunt, nabt = dtpg.ffr_mode(False)

    time0 = time.process_time()
    print('----------------------------------------------------')
    print('{}'.format(os.path.basename(file_name)), flush = True)
    fault_list = MinPatMgr.fault_reduction(dtpg.fault_list, network, fault_type, 'red1,red2', True)
    time1 = time.process_time()
    print('red1,red2 CPU time: {:8.2f}'.format(time1 - time0), flush = True)

    fault_list = MinPatMgr.fault_reduction(dtpg.fault_list, network, fault_type, 'red2', True)
    time2 = time.process_time()
    print('red2      CPU time: {:8.2f}'.format(time2 - time1), flush = True)
    print('', flush = True)

    fault_list = MinPatMgr.fault_reduction(dtpg.fault_list, network, fault_type, 'red3', True)
    time3 = time.process_time()
    print('red3      CPU time: {:8.2f}'.format(time3 - time3), flush = True)
    print('', flush = True)


def main() :

    print('reduce_test.py: main()')

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

    for file_name in args.file_list :
        dtpg1(file_name, fault_type)


if __name__ == '__main__' :
    main()
