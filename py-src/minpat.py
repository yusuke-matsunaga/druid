#! /usr/bin/env py_druid

"""テストパタン圧縮 を行うスクリプト

:file: minpat.py
:author: Yusuke Matsunaga (松永 裕介)
:Copyright: (C) 2024 Yusuke Matsunaga
 All rights reserved.
"""

import argparse
import time
from druid.types import TpgNetwork, FaultType, FaultStatus
from druid.dtpg import DtpgMgr, DtpgResult
from druid.fsim import Fsim

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
parser.add_argument('-v', '--verbose',
                    action='store_true',
                    help='get verbose')

args = parser.parse_args()
if not args:
    exit(1)

blif = False
iscas89 = False
if args.blif:
    blif = True
if args.iscas89:
    iscas89 = True
if not blif and not iscas89:
    blif = True
    
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
    exit(1)
verbose = args.verbose

if blif:
    network = TpgNetwork.read_blif(filename, fault_type)
elif iscas89:
    network = TpgNetwork.read_iscas89(filename, fault_type)
else:
    assert False

fault_list = network.rep_fault_list
dtpg = DtpgMgr(network, fault_list)

option = {
    "dtpg_type": "mffc",
    "just_type": "just1",
    }

start_time = time.process_time()

fsim = Fsim(network, fault_list, 3, False)


class DetCallBack:

    def __init__(self, fsim):
        self.__fsim = fsim

    def __call__(self, dtpg_mgr, f, tv):
        self.__fsim.set_skip(f)
        f_list = self.__fsim.sppfp(tv)
        for fault, __dummy in f_list:
            if dtpg_mgr.dtpg_result(fault).status == FaultStatus.Undetected:
                dtpg_mgr.set_dtpg_result(fault, DtpgResult.detected(tv))
                self.__fsim.set_skip(fault)
                

det_callback = DetCallBack(fsim)

#dtpg.run(option=option, det_func=det_callback)
dtpg.run(option=option)

end_time = time.process_time()

dtpg_time = end_time - start_time

fault_list = []
for f in network.rep_fault_list:
    r = dtpg.dtpg_result(f)
    if r.status == FaultStatus.Detected:
        fault_list.append(f)
print(f'# of faults:  {len(fault_list):6d}')
tv_list = dtpg.testvector_list
print(f'# of tv_list: {len(tv_list):6d}')
print(f'CPU time:     {dtpg_time:9.2f}')
