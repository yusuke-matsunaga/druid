#! /usr/bin/env python3

"""TpgNetwork のテストプログラム

:file: tpg_network_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork


def test_read_blif():
    TESTDATA_DIR = os.environ.get('TESTDATA_DIR')
    filename = os.path.join(TESTDATA_DIR, 's38584.blif')
    network = TpgNetwork.read_blif(filename)

    assert network.node_num == 23900
    assert network.input_num == 13
    assert network.output_num == 278
    assert network.ppi_num == 1465
    assert network.ppo_num == 1730
    assert network.mffc_num == 4689
    assert network.ffr_num == 5676
    assert network.dff_num == 1452

    for ffr in network.ffr_list:
        for fault in ffr.fault_list:
            print(fault)
            
def test_read_bench():
    TESTDATA_DIR = os.environ.get('TESTDATA_DIR')
    filename = os.path.join(TESTDATA_DIR, 'b01.bench')
    network = TpgNetwork.read_bench(filename)
    
    assert network.node_num == 60
    assert network.input_num == 3
    assert network.output_num == 2
    assert network.ppi_num == 8
    assert network.ppo_num == 7
    assert network.mffc_num == 21
    assert network.ffr_num == 24
    assert network.dff_num == 5
    
