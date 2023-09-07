
### @file tpgfault.pxi
### @brief tpgfault の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_TpgFault cimport TpgFault as CXX_TpgFault
from CXX_TpgNode cimport TpgNode as CXX_TpgNode


### @brief TpgFault の Python バージョン
cdef class TpgFault :
    cdef const CXX_TpgFault* _thisptr

    ### @brief 初期化
    def __cinit__(self) :
        self._thisptr = NULL

    ### @brief 終了処理
    def __dealloc__(self) :
        # なにもしない．
        pass

    ### @brief 適正な値を持っている時に True を返す．
    @property
    def is_valid(self) :
        return self._thisptr != NULL

    ### @brief ID番号を返す．
    @property
    def id(self) :
        if not self.is_valid :
            return -1
        return self._thisptr.id()

    ### @brief 入力側のノードを返す．
    @property
    def inode(self) :
        cdef const CXX_TpgNode* c_node
        if not self.is_valid :
            return None
        c_node = self._thisptr.tpg_inode()
        return to_TpgNode(c_node)

    ### @brief 出力側のノードを返す．
    @property
    def onode(self) :
        cdef const CXX_TpgNode* c_node
        if not self.is_valid :
            return None
        c_node = self._thisptr.tpg_onode()
        return to_TpgNode(c_node)

    ### @brief ステムの故障の時に True を返す．
    @property
    def is_stem_fault(self) :
        return self.is_valid and self._thisptr.is_stem_fault()

    ### @brief ブランチの故障の時に True を返す．
    @property
    def is_branch_fault(self) :
        return self.is_valid and self._thisptr.is_branch_fault()

    ### @brief オリジナルの故障位置を返す．
    ###
    ### is_branch_fault == True のときのみ意味を持つ．
    @property
    def fault_pos(self) :
        if not self.is_valid :
            return -1
        return self._thisptr.fault_pos()

    ### @brief TpgNode 上の故障位置を返す．
    ###
    ### is_branch_fault == True のときのみ意味を持つ．
    @property
    def tpg_pos(self) :
        if not self.is_valid :
            return -1
        return self._thisptr.tpg_pos()

    ### @brief 故障値を返す．
    @property
    def val(self) :
        if not self.is_valid :
            return -1
        return self._thisptr.val()

    ### @brief この故障を表す文字列を返す．
    @property
    def str(self) :
        cdef string c_str
        if not self.is_valid :
            return None
        c_str = self._thisptr.str()
        return c_str.decode('UTF-8')

    ### @brief 代表故障の時に True を返す．
    @property
    def is_rep(self) :
        return self.is_valid and self._thisptr.is_rep()

    ### @brief 代表故障を返す．
    @property
    def rep_fault(self) :
        cdef const CXX_TpgFault* c_fault
        if not self.is_valid :
            return None
        c_fault = self._thisptr.rep_fault()
        return to_TpgFault(c_fault)


### @brief CXX_TpgFault への変換
cdef const CXX_TpgFault* from_TpgFault(TpgFault fault) :
    return fault._thisptr


### @brief CXX_TpgFault からの変換
cdef to_TpgFault(const CXX_TpgFault* c_fault) :
    if c_fault == NULL :
        return None
    ans = TpgFault()
    ans._thisptr = c_fault
    return ans
