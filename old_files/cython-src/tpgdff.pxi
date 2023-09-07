#
# @file tpgdff.pxi
# @brief TpgDff の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from CXX_TpgDff cimport TpgDff as CXX_TpgDff
from CXX_TpgNode cimport TpgNode as CXX_TpgNode


## @brief TpgDff の Python バージョン
cdef class TpgDff :
    cdef int _id
    cdef const CXX_TpgNode* _input
    cdef const CXX_TpgNode* _output
    cdef const CXX_TpgNode* _clock
    cdef const CXX_TpgNode* _clear
    cdef const CXX_TpgNode* _preset

    ## @brief 初期化
    def __cinit__(self) :
        self._id = -1
        self._input = NULL
        self._output = NULL
        self._clock = NULL
        self._clear = NULL
        self._preset = NULL

    ## @brief 終了処理
    def __dealloc__(self) :
        # なにもしない
        pass

    ## @brief ID番号を返す．
    @property
    def id(self) :
        return self._id

    ## @brief 入力のノードを返す．
    @property
    def input(self) :
        return to_TpgNode(self._input)

    ## @brief 出力のノードを返す．
    @property
    def output(self) :
        return to_TpgNode(self._output)

    ## @brief クロックのノードを返す．
    @property
    def clock(self) :
        return to_TpgNode(self._clock)

    ## @brief クリア端子のノードを返す．
    @property
    def clear(self) :
        return to_TpgNode(self._clear)

    ## @brief プリセット端子のノードを返す．
    @property
    def preset(self) :
        return to_TpgNode(self._preset)

## @brief CXX_TpgDff からの変換
cdef to_TpgDff(const CXX_TpgDff* c_dff) :
    if c_dff == NULL :
        return None
    ans = TpgDff()
    ans._id = c_dff.id()
    ans._input = c_dff.input()
    ans._output = c_dff.output()
    ans._clock = c_dff.clock()
    ans._clear = c_dff.clear()
    ans._preset = c_dff.preset()
    return ans
