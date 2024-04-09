#! /usr/bin/env python3

"""TpgNetwork のテストプログラム

:file: tpg_network_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import os
from druid.types import TpgNetwork, FaultType
from make_filename import make_filename


def test_read_blif():
    filename = make_filename('s38584.blif')
    network = TpgNetwork.read_blif(filename, FaultType.StuckAt)

    assert network.node_num == 22447
    assert network.input_num == 12
    assert network.output_num == 278
    assert network.ppi_num == 1464
    assert network.ppo_num == 1730
    assert network.mffc_num == 4689
    assert network.ffr_num == 5676
    assert network.dff_num == 1452
            
def test_read_bench():
    filename = make_filename('b01.bench')
    network = TpgNetwork.read_bench(filename, FaultType.StuckAt)
    
    assert network.node_num == 54
    assert network.input_num == 2
    assert network.output_num == 2
    assert network.ppi_num == 7
    assert network.ppo_num == 7
    assert network.mffc_num == 21
    assert network.ffr_num == 24
    assert network.dff_num == 5
    
