#! /usr/bin/env py_druid

"""DTPG を行うクラス

:file: dtpg.py
:author: Yusuke Matsunaga (松永 裕介)
:Copyright: (C) 2018 Yusuke Matsunaga
 All rights reserved.
"""

import argparse
import time
from druid.types import TpgNetwork, FaultType, FaultStatus
from druid.dtpg import DtpgMgr
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

dtpg.run(option=option)

tv_list = dtpg.testvector_list

det_fault_list = []
for f in fault_list:
    r = dtpg.dtpg_result(f)
    if r.status == FaultStatus.Detected:
        det_fault_list.append(f)
print(f'# of faults: {len(det_fault_list)}')
print(f'# of tv_list: {len(tv_list)}')

fsim = Fsim(network, det_fault_list, 2, False)

start_time = time.process_time()

max_fid = max([ f.id for f in det_fault_list ]) + 1
fgmap = [ None for _ in range(max_fid) ]
for f in det_fault_list:
    fgmap[f.id] = 0
count_array = [ len(det_fault_list) ]
for i, tv in enumerate(tv_list):
    tmp_list = fsim.sppfp(tv)
    dbits_dict = {}
    for fault, dbits in tmp_list:
        fid = fault.id
        key = (fgmap[fid], dbits)
        if key not in dbits_dict:
            g = len(count_array)
            count_array.append(0)
            dbits_dict[key] = g
        else:
            g = dbits_dict[key]
        old_g = fgmap[fid]
        fgmap[fid] = g
        count_array[old_g] -= 1
        count_array[g] += 1
    ng = len(count_array)
    for f in det_fault_list:
        fid = f.id
        g = fgmap[fid]
        if count_array[g] <= 1:
            fgmap[fid] = -1
            fsim.set_skip(f)

    fg_list = [[] for _ in range(ng)]
    for f in det_fault_list:
        fid = f.id
        g = fgmap[fid]
        if g >= 0:
            fg_list[g].append(f)

    if verbose:
        print()
        for i, fg in enumerate(fg_list):
            if len(fg) > 0:
                print(f'G#{i}:', end='')
                for f in fg:
                    print(f' {f}', end='')
                print()

gmap = {}
last_g = 0
for g in fgmap:
    if g is not None and g >= 0:
        if g not in gmap:
            gmap[g] = last_g
            last_g += 1

fg_list = [ [] for _ in range(last_g) ]
for f in fault_list:
    g = fgmap[f.id]
    if g is not None and g >= 0:
        new_g = gmap[g]
        fg_list[new_g].append(f)

end_time = time.process_time()

print(f'# of groups: {last_g}')
c = 0
for fg in fg_list:
    n = len(fg)
    c += (n * (n - 1)) // 2
print(f'# of paris: {c}')
print(f'CPU time:   {end_time - start_time:0.2f}')
    
