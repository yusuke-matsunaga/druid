#! /usr/bin/env python3

"""

:file: dtpg_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork, FaultType, FaultStatus
from druid.dtpg import DtpgMgr


def test_dtpg():
    TESTDATA_DIR = os.environ.get('TESTDATA_DIR')
    filename = os.path.join(TESTDATA_DIR, 's27.blif')
    network = TpgNetwork.read_blif(filename)
    fault_type = FaultType.TransitionDelay
    mgr = DtpgMgr(network, fault_type, "mffc")
    mgr.add_base_dop()
    mgr.add_base_uop()
    mgr.run()
    assert mgr.detect_count == 32
    assert mgr.untest_count == 0
    assert mgr.abort_count == 0
