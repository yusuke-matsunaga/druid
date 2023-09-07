
### @file nodevallist.pxi
### @brief NodeValList の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.


from CXX_NodeValList cimport NodeVal as CXX_NodeVal
from CXX_NodeValList cimport NodeValList as CXX_NodeValList
from CXX_NodeValList cimport check_conflict as CXX_check_conflict
from CXX_NodeValList cimport check_contain as CXX_check_contain
from CXX_NodeValList cimport compare as CXX_compare
from libcpp.vector cimport vector


### @brief NodeValList の Python バージョン
cdef class NodeValList :
    cdef CXX_NodeValList _this


    ### @brief クリアする．
    def clear(NodeValList self) :
        self._this.clear()

    ### @brief 値を追加する．
    def add(NodeValList self, TpgNode node, int time, bool val) :
        cdef const CXX_TpgNode* c_node = node._thisptr
        self._this.add(c_node, time, val)

    ### @brief マージする．
    def merge(NodeValList self, NodeValList src_list) :
        self._this.merge(src_list._this)

    ### @brief 差分を計算する．
    def diff(NodeValList self, NodeValList src_list) :
        self._this.diff(src_list._this)

    ### @brief 要素数を返す．
    def size(NodeValList self) :
        return self._this.size()

    ### @brief 要素を返す．
    def elem(NodeValList self, int pos) :
        return to_NodeVal(self._this.elem(pos))


### @brief 2つの割当リストが矛盾しているか調べる．
cdef check_conflict(NodeValList right, NodeValList left) :
    return CXX_check_conflict(left._this, right._this)


### @brief 2つの割当リストの包含関係を調べる．
cdef check_contain(NodeValList right, NodeValList left) :
    return CXX_check_contain(left._this, right._this)


### @brief 2つの割当リストを比較する．
cdef compare(NodeValList right, NodeValList left) :
    return CXX_compare(left._this, right._this)


### @brief C++ の NodeValList から Python の NodeValList に変換する．
cdef to_NodeValList(CXX_NodeValList c_nodeval_list) :
    ans = NodeValList()
    ans._this = c_nodeval_list
    return ans
