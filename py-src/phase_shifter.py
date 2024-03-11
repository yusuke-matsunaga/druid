#! /usr/bin/env python3

"""

:file: phase_shifter.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2024 Yusuke Matsunaga, All rights reserved.
"""

from druid.types import TestVector, Val3


class PhaseShifter:
    """Phase-Shifter を表すクラス

    具体的には元となるLFSRのビット列をPPIのビットベクタに変換するもの．
    各入力はLFSR中から選ばれた複数のビットのXORで計算される．
    """

    def __init__(self, input_num, dff_num, has_prev_state, config_list):
        assert len(config_list) == (input_num + dff_num)
        self._input_num = input_num
        self._dff_num = dff_num
        self._has_prev_state = has_prev_state
        self._config_list = config_list[:]

    def convert(self, bits):
        """LFSRのビット列をPPIのビットベクタに変換する．
        :param list[int] bits: LFSRのビット列
        :return: 変換されたテストベクタ
        """
        tv = TestVector(self._input_num, self._dff_num, self._has_prev_state)
        for i in range(self._input_num):
            config = self._config_list[i]
            v = 0
            for c in config:
                v ^= bits[c]
            v3 = Val3(v)
            tv.set_input_val(i, v3)
            if self._has_prev_state:
                tv.set_aux_input_val(i, v3)
        for i in range(self._dff_num):
            config = self._config_list[i + self._input_num]
            v = 0
            for c in config:
                v ^= bits[c]
            v3 = Val3(v)
            tv.set_dff_val(i, v3)
        return tv
