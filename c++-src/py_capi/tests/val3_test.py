#! /usr/bin/env python3

"""Val3 のテストプログラム

:file: val3_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import itertools
from druid.types import Val3


def test_constructor1():
    val = Val3(0)
    assert val == Val3._0

def test_constructor2():
    val = Val3(1)
    assert val == Val3._1

def test_constructor3():
    val = Val3("0")
    assert val == Val3._0

def test_constructor4():
    val = Val3("1")
    assert val == Val3._1

def test_constructor5():
    val = Val3("x")
    assert val == Val3._X

def test_constructor6():
    val = Val3("X")
    assert val == Val3._X

def test_constructor7():
    val = Val3("?")
    assert val == Val3._X

def test_bad_constructor1():
    with pytest.raises(Exception) as e:
        _ = Val3(3)
    assert e.type == TypeError
    assert str(e.value) == "argument 1 must be 0 or 1"

def test_bad_constructor2():
    with pytest.raises(Exception) as e:
        _ = Val3("2")
    assert e.type == TypeError
    assert str(e.value) == 'argument 1 must be "0", "1", "x", "X" or "?"'
        
def test_negation_x():
    assert ~Val3._X == Val3._X

def test_negation_0():
    assert ~Val3._0 == Val3._1

def test_negation_1():
    assert ~Val3._1 == Val3._0

val3_list = (Val3._0, Val3._1, Val3._X)

# Val3 の値の対を生成するジェネレーター関数
def all_val3_pair():
    return itertools.product(val3_list, val3_list)

# Val3 の対の名前を生成するジェネレーター関数
def all_val3_pair_name():
    for v1, v2 in all_val3_pair():
        yield f'{v1}-{v2}'

@pytest.fixture(params=all_val3_pair(), ids=all_val3_pair_name())
def val3_pair(request):
    return request.param

def test_and(val3_pair):
    v1, v2 = val3_pair
    v = v1 & v2
    if v1 == Val3._0:
        exp_v = Val3._0
    elif v2 == Val3._0:
        exp_v = Val3._0
    elif v1 == Val3._X:
        exp_v = Val3._X
    elif v2 == Val3._X:
        exp_v = Val3._X
    else:
        exp_v = Val3._1
    assert v == exp_v

def test_or(val3_pair):
    v1, v2 = val3_pair
    v = v1 | v2
    if v1 == Val3._1:
        exp_v = Val3._1
    elif v2 == Val3._1:
        exp_v = Val3._1
    elif v1 == Val3._X:
        exp_v = Val3._X
    elif v2 == Val3._X:
        exp_v = Val3._X
    else:
        exp_v = Val3._0
    assert v == exp_v
