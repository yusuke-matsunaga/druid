#! /usr/bin/env python3

"""

:file: fsim_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork, TpgFaultMgr, FaultType, FaultStatus, TestVector
from druid.fsim import Fsim
from druid.ymbase import Mt19937
from make_filename import make_filename


def ppsfp_callback(index, tv, fault):
    print(f'{index}: {fault}')
    return True

def test_fsim():
    filename = make_filename('s27.blif')
    network = TpgNetwork.read_blif(filename)
    fault_mgr = TpgFaultMgr()
    fault_mgr.gen_fault_list(network, FaultType.TransitionDelay)
    fsim = Fsim()
    fsim.initialize(network, True, 2)
    fsim.set_fault_list(fault_mgr.rep_fault_list())

    input_num = network.input_num
    dff_num = network.dff_num
    tv_list = []
    rg = Mt19937()
    for _ in range(1000):
        tv = TestVector(input_num, dff_num, True)
        tv.set_from_random(rg)
        tv_list.append(tv)

    fsim.ppsfp(tv_list, ppsfp_callback)
