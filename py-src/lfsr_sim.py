#! /usr/bin/env py_druid

"""LFSR の故障シミュレーションを行うクラス

:file: lfsr_sim.py
:author: Yusuke Matsunaga (松永 裕介)
:Copyright: (C) 2024 Yusuke Matsunaga
 All rights reserved.
"""

import random
import time
from druid.types import TpgNetwork, TpgFaultMgr, FaultType, FaultStatus
from druid.dtpg import DtpgMgr
from druid.fsim import Fsim
from lfsr import LFSR
from phase_shifter import PhaseShifter


def psgen(input_num, dff_num, has_prev_state, lfsr_bitlen, K):
    """Phase-Shifter を生成する．

    :param int ppi_num: 入力数
    :param int lfsr_bitlen: LFSR のビット長
    :param int K: 一つの入力あたりのタップ数
    :return: 各入力のタップ位置のリストのリストを返す．
    """
    ppi_num = input_num + dff_num
    input_config_list = [ random.sample(range(lfsr_bitlen), K) for _ in range(ppi_num) ]
    return PhaseShifter(input_num, dff_num, has_prev_state, input_config_list)

    
if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()

    parser.add_argument('filename')
    parser.add_argument('--fault_type',
                        type=str,
                        help="specify the fault type('stuck_at' or 'transition_delay')")
    parser.add_argument('--blif',
                        action='store_true',
                        help='read blif file [default]')
    parser.add_argument('--iscas89',
                        action='store_true',
                        help='read ISCAS89(.bench) file')
    parser.add_argument('--pat_num',
                        type=int,
                        help='specify the number of patterns',
                        required=True)
    parser.add_argument('--loop_num',
                        type=int,
                        help='specify the loop counts',
                        default=1)
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

    if args.blif:
        network = TpgNetwork.read_blif(filename)
    elif args.iscas89:
        network = TpgNetwork.read_bench(filename)
    else:
        print('Neither --blif nor --iscas89 are specified')
        exit(1)
        
    fault_mgr = TpgFaultMgr()
    fault_mgr.gen_fault_list(network, fault_type)

    fsim = Fsim()
    fsim.initialize(network, fault_type, 2);
    fsim.set_fault_list(fault_mgr.rep_fault_list())

    lfsr_bitlen = 100
    K = 4
    input_num = network.input_num
    dff_num = network.dff_num
    has_prev_state = fault_type == FaultType.TransitionDelay
    phase_shifter = psgen(input_num, dff_num, has_prev_state, lfsr_bitlen, K)
    lfsr = LFSR(lfsr_bitlen, [99, 62])
    
    start_time = time.process_time()

    nf = len(fault_mgr.rep_fault_list())
    
    def cb(f, dbits):
        fsim.set_skip(f)
        fault_mgr.set_status(f, FaultStatus.Detected)
        #nf -= 1

    for j in range(args.loop_num):
        seed = random.choices([0, 1], k=lfsr_bitlen)
        lfsr.set(seed)
        tv_buff = []
        base = 0
        for i in range(args.pat_num):
            tv = phase_shifter.convert(lfsr.bits)
            lfsr.shift()
            tv_buff.append(tv)
            n = len(tv_buff)
            if n == fsim.ppsfp_bitlen or base + n == args.pat_num:
                fsim.ppsfp(tv_buff, cb)
                base += n
                tv_buff = []
    end_time = time.process_time()

    print(f'CPU time:   {end_time - start_time:0.2f}')
    print(f'# Patters:  {i + 1}')
    nf = 0
    for f in fault_mgr.rep_fault_list():
        if fault_mgr.get_status(f) == FaultStatus.Undetected:
            nf += 1
    print(f'# Total Faults:  {len(fault_mgr.rep_fault_list())}')
    print(f'# Remain Faults: {nf}')
