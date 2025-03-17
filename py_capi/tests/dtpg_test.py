#! /usr/bin/env python3

"""

:file: dtpg_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork, FaultType
from druid.dtpg import DtpgMgr
from make_filename import make_filename


def test_dtpg():
    filename = make_filename('s27.blif')
    network = TpgNetwork.read_blif(filename, FaultType.TransitionDelay)
    fault_list = network.rep_fault_list
    print(f"len(fault_list.size()) =  {len(fault_list)}")
    option = {
        'group_mode': 'ffr'
        }
    mgr = DtpgMgr(network, fault_list)
    stats = mgr.run(option=option)
    assert mgr.detected_count == 32
    assert mgr.untestable_count == 0
    assert mgr.undetected_count == 0
