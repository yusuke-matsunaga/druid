#! /usr/bin/env py_druid

"""DTPG を行うクラス

:file: dtpg.py
:author: Yusuke Matsunaga (松永 裕介)
:Copyright: (C) 2018 Yusuke Matsunaga
 All rights reserved.
"""

from druid.types import TpgNetwork, TpgFaultMgr, FaultType
from druid.dtpg import DtpgMgr
from druid.fsim import Fsim

filename = "/home/yusuke/data/public/ISCAS.s/COMBI/s35932.blif"

network = TpgNetwork.read_blif(filename)
fault_mgr = TpgFaultMgr()
fault_mgr.gen_fault_list(network, FaultType.StuckAt)

option = {
    "dtpg_type": "mffc",
    "just_type": "just1",
    "dop": [ "base", "drop", "tvlist" ]
    }
dtpg = DtpgMgr(network, fault_mgr, option)

dtpg.run()

tv_list = dtpg.tv_list

fault_list = fault_mgr.fault_list()

print(f'# of faults: {len(fault_list)}')
print(f'# of tv_list: {len(tv_list)}')

fsim = Fsim()
fsim.initialize(network, False, 2);
fsim.set_fault_list(fault_list)

for i, tv in enumerate(tv_list):
    print(f'#{i}')
    det_fault_list = fsim.sppfp(tv)
    dbits_dict = {}
    for fault, dbits in det_fault_list:
        if dbits not in dbits_dict:
            dbits_dict[dbits] = []
        dbits_dict[dbits].append(fault)
    for fault_list in dbits_dict.values():
        for fault in fault_list:
            print(f' {fault}', end='')
        print()
    
