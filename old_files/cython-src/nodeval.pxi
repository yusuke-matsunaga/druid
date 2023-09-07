
### @file nodeval.pxi
### @brief NodeVal の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.


from libcpp cimport bool
from CXX_NodeValList cimport NodeVal as CXX_NodeVal


### @brief NodeVal の Python バージョン
cdef class NodeVal :
    cdef TpgNode _node
    cdef int _time
    cdef bool _val

    ### @brief 初期化
    def __init__(NodeVal self, TpgNode node = None, int time = 0, bool val = False) :
        self._node = node
        self._time = time
        self._val = val

    ### @brief ノードを返す．
    @property
    def node(NodeVal self) :
        return self._node

    ### @brief 時刻を返す．
    @property
    def time(NodeVal self) :
        return self._time

    ### @brief 値を返す．
    @property
    def val(NodeVal self) :
        return self._val


### @brief C++ の NodeVal を Python の NodeVal に変換する．
cdef to_NodeVal(CXX_NodeVal c_nodeval) :
    node = to_TpgNode(c_nodeval.node())
    return NodeVal(node, c_nodeval.time(), c_nodeval.val())
