#! /usr/bin/env py_druid

"""CondGen を行うプログラム

:file: condgen.py
:author: Yusuke Matsunaga (松永 裕介)
:Copyright: (C) 2018 Yusuke Matsunaga
 All rights reserved.
"""

import argparse
import time
from druid.types import TpgNetwork, FaultType, FaultStatus
from druid.dtpg import DtpgMgr
from druid.fsim import Fsim
from druid.condgen import root_cond, fault_cond

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
parser.add_argument('--limit',
                    type=int,
                    help="loop count limit")
mode_group = parser.add_mutually_exclusive_group()
mode_group.add_argument('--ffr',
                        action='store_true',
                        help='FFR mode')
mode_group.add_argument('--fault',
                        action='store_true',
                        help='fault mode')
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
verbose = args.verbose

if blif:
    network = TpgNetwork.read_blif(filename, fault_type)
elif iscas89:
    network = TpgNetwork.read_iscas89(filename, fault_type)
else:
    assert False

if args.limit:
    limit = args.limit
else:
    limit = 1

if args.ffr:
    ffr_mode = True
else:
    ffr_mode = False
    
fault_list = network.rep_fault_list

start_time = time.process_time()

if ffr_mode:
    def callback(ffr, cond, count, time):
        print(f'FFR#{ffr.ffr_id}: {count}, {time}')
        
    root_cond(network=network, limit=limit, callback=callback)
else:
    def callback(fault, cond, count, time):
        print(f'{fault}: {count}, {time}')
        
    fault_cond(network=network, fault_list=fault_list,
               limit=limit, callback=callback)

end_time = time.process_time()

print(f'CPU time:   {end_time - start_time:0.2f}')
    
