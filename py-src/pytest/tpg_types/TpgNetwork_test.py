#! /usr/bin/env python3

""" TpgNetwork のテストプログラム

:file: TpgNetworkTest.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from druid.tpg_types import TpgNetwork, FaultType
from ymworks.bn import BnModel
from ymworks.logic import PrimType


@pytest.fixture
def bn_model():
    return BnModel()

def make_input(model, input_num):
    return [ model.new_input(name=f'input{i}') for i in range(input_num) ]

def make_primitive(model, prim_type, input_num):
    input_list = make_input(model, input_num)
    node = model.new_primitive(prim_type, input_list)
    model.new_output(node, name="output")
    return TpgNetwork.from_bn(model, FaultType.StuckAt)

def test_constructor1():
    network = TpgNetwork()

    assert not network.is_valid()

def test_xor2(bn_model):
    network = make_primitive(bn_model, PrimType.Xor, 2)
