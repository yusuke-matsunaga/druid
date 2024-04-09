#! /usr/bin/env python3

"""

:file: dtpg_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork, TpgFaultStatusMgr, FaultType, FaultStatus
from druid.dtpg import DtpgMgr
from make_filename import make_filename


def test_dtpg():
    filename = make_filename('s27.blif')
    network = TpgNetwork.read_blif(filename, FaultType.TransitionDelay)
    fault_list = network.rep_fault_list
    fault_mgr = TpgFaultStatusMgr(fault_list)
    option = {
        'dtpg_type': 'mffc'
        }
    def dfunc(f, tv):
        pass
    def ufunc(f):
        pass
    def afunc(f):
        pass
    DtpgMgr.run(network, fault_mgr, dfunc, ufunc, afunc, option)
    assert mgr.detect_count == 32
    assert mgr.untest_count == 0
    assert mgr.abort_count == 0
