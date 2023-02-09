#! /usr/bin/env python3

"""

:file: dtpg_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork, FaultType, FaultStatus
from druid.dtpg import DtpgFFR, DtpgMFFC


def test_dtpg():
    TESTDATA_DIR = os.environ.get('TESTDATA_DIR')
    filename = os.path.join(TESTDATA_DIR, 's27.blif')
    network = TpgNetwork.read_blif(filename)
    fault_type = FaultType.TransitionDelay
    n_faults = 0
    n_detected = 0
    n_untestable = 0
    n_abort = 0
    for mffc in network.mffc_list:
        dtpg = DtpgMFFC(network, fault_type, mffc)
        for fault in mffc.fault_list:
            r, tv = dtpg.gen_pattern(fault)
            n_faults += 1
            if r == FaultStatus.Detected:
                n_detected += 1
            elif r == FaultStatus.Untestable:
                n_untestable += 1
            else:
                n_abort += 1
    assert n_faults == 32
    assert n_detected == 32
    assert n_untestable == 0
    assert n_abort == 0
