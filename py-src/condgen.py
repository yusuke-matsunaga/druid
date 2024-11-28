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
read_group = parser.add_mutually_exclusive_group()
read_group.add_argument('--blif',
                        action='store_true',
                        help='read blif file [default]')
read_group.add_argument('--iscas89',
                        action='store_true',
                        help='read ISCAS89(.bench) file')
read_group.add_argument('--bench',
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
parser.add_argument('--method2',
                    action='store_true',
                    help="use method2")
parser.add_argument('-v', '--verbose',
                    action='store_true',
                    help='get verbose')

args = parser.parse_args()
if not args:
    exit(1)

blif = False
bench = False
if args.blif:
    blif = True
if args.iscas89:
    bench = True
if args.bench:
    bench = True
if not blif and not bench:
    blif = True
    
filename = args.filename
fault_type = None
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
elif bench:
    network = TpgNetwork.read_bench(filename, fault_type)
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

option = {}
if args.method2:
    option['method2'] = True
    
if ffr_mode:
    result_list = []
    
    def callback(ffr, cond, count, time):
        #print(f'FFR#{ffr.ffr_id}: {count}, {time}')
        result_list.append((ffr.ffr_id, count))
        
    root_cond(network=network,
              limit=limit,
              callback=callback,
              option=option)
            
else:
    result_list = []
    
    def callback(fault, cond, count, time):
        print(f'{fault}: {count}, {time}')
        result_list.append((fault.id, count))
        
    fault_cond(network=network,
               fault_list=fault_list,
               limit=limit,
               callback=callback,
               option=option)

end_time = time.process_time()

n1 = 0
n10 = 0
nmax = 0
total_count = 0
for _, count in result_list:
    total_count += count
    if count == 1:
        n1 += 1
    elif count <= 10:
        n10 += 1
    elif count == limit:
        nmax += 1

print(f'=1: {n1:4}, <=10: {n10:4}, ={limit}: {nmax:4}, total {total_count}')
print(f'CPU time:   {end_time - start_time:0.2f}')
    
