#! /usr/bin/env py_druid

"""Classify のテスト

:file: classify_test.py
:author: Yusuke Matsunaga (松永 裕介)
:Copyright: (C) 2024 Yusuke Matsunaga
 All rights reserved.
"""

import argparse
import time
from druid.types import TpgNetwork, TpgFaultMgr, TestVector, FaultType, FaultStatus
from druid.dtpg import DtpgMgr
from druid.fsim import Fsim
from druid.main import classify
from druid.ymbase import Mt19937


def run_classify(network, fault_list, fault_type, tv_list, drop, ppsfp, multi, name):
    
    start_time = time.thread_time()
    fg_list = classify(network, fault_list, fault_type, tv_list, drop, ppsfp, multi,
                       verbose=True)
    end_time = time.thread_time()

    c = 0
    for fg in fg_list:
        n = len(fg)
        c += (n * (n - 1)) // 2
    print(f'# of groups: {len(fg_list)}')
    print(f'# of paris:  {c}')
    print(f'Classify({name}) time:   {end_time - start_time:0.2f}')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()

    parser.add_argument('filename')
    parser.add_argument('tv_filename',
                        type=str,
                        help="specify the testvector fileneam")
    parser.add_argument('--fault_type',
                        type=str,
                        help="specify the fault type('stuck_at' or 'transition_delay')")
    parser.add_argument('--blif',
                        action='store_true',
                        help='read blif file [default]')
    parser.add_argument('--iscas89',
                        action='store_true',
                        help='read ISCAS89(.bench) file')
    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='get verbose')

    args = parser.parse_args()
    if not args:
        exit(1)
    
    filename = args.filename
    if args.fault_type:
        fault_type_str = args.fault_type
    else:
        fault_type_str = 'stuck_at'
    if fault_type_str == 'stuck_at':
        fault_type = FaultType.StuckAt
    elif fault_type_str == 'transition_delay':
        fault_type = FaultType.TransitionDelay
    else:
        print(f'{fault_type_str}: illegal value for fault_type')
    verbose = args.verbose

    blif = False
    iscas89 = False
    if args.blif:
        blif = True
    elif args.iscas89:
        iscas89 = True
    else:
        # デフォルトフォールバック
        blif = True

    if blif:
        network = TpgNetwork.read_blif(filename)
    elif iscas89:
        network = TpgNetwork.read_bench(filename)
    else:
        assert False
        
    fault_mgr = TpgFaultMgr()
    fault_mgr.gen_fault_list(network, fault_type)

    input_num = network.input_num
    dff_num = network.dff_num
    has_prev_state = fault_type == FaultType.TransitionDelay
    tv_filename = args.tv_filename
    tv_list = []
    with open(tv_filename, "rt") as fin:
        for line in fin:
            line = line.rstrip()
            tv = TestVector.from_hex(line, input_num, dff_num, has_prev_state)
            tv_list.append(tv)

    fsim = Fsim()
    fsim.initialize(network, fault_type, 2);
    fsim.set_fault_list(fault_mgr.rep_fault_list())
    fault_list = []

    def callback(f, dbits):
        fsim.set_skip(f)
        fault_list.append(f)
    
    base = 0
    tv_buff = []
    NTV = len(tv_list)
    for tv in tv_list:
        tv_buff.append(tv)
        n = len(tv_buff)
        if n == 64 or base + n == NTV:
            fsim.ppsfp(tv_buff, callback)
            base += len(tv_buff)
            tv_buff = []
        
    print(f'# of faults: {len(fault_list)}')
    print(f'# of tv_list: {len(tv_list)}')

    run_classify(network, fault_list, fault_type, tv_list,
                 False, False, False, "no-drop, sppfp")
    run_classify(network, fault_list, fault_type, tv_list,
                 False, True, False, "no-drop, ppsfp")
    run_classify(network, fault_list, fault_type, tv_list,
                 True, False, False, "drop, sppfp")
    run_classify(network, fault_list, fault_type, tv_list,
                 True, True, False, "drop, ppsfp")

    run_classify(network, fault_list, fault_type, tv_list,
                 False, False, True, "no-drop, sppfp, multi")
    run_classify(network, fault_list, fault_type, tv_list,
                 False, True, True, "no-drop, ppsfp, multi")
    run_classify(network, fault_list, fault_type, tv_list,
                 True, False, True, "drop, sppfp, multi")
    run_classify(network, fault_list, fault_type, tv_list,
                 True, True, True, "drop, ppsfp, multi")

