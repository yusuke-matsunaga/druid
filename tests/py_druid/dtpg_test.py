#! /usr/bin/env python3

"""

:file: dtpg_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork, TpgFaultMgr, FaultType, FaultStatus
from druid.dtpg import DtpgMgr
from make_filename import make_filename


def test_dtpg():
    filename = make_filename('s27.blif')
    network = TpgNetwork.read_blif(filename)
    fault_mgr = TpgFaultMgr()
    fault_mgr.gen_fault_list(network, FaultType.TransitionDelay)
    mgr = DtpgMgr(network, fault_mgr, "mffc")
    mgr.add_base_dop()
    mgr.add_base_uop()
    mgr.run()
    assert mgr.detect_count == 32
    assert mgr.untest_count == 0
    assert mgr.abort_count == 0
