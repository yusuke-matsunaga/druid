
### @file tpgnode.pxi
### @brief TpgNode の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp.string cimport string
from CXX_TpgNode cimport TpgNode as CXX_TpgNode
from CXX_TpgDff cimport TpgDff as CXX_TpgDff
from CXX_TpgFault cimport TpgFault as CXX_TpgFault


### @brief TpgNode の Python バージョン
cdef class TpgNode :
    cdef const CXX_TpgNode* _thisptr

    ### @brief 初期化
    def __cinit__(self) :
        _thisptr = NULL

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
            return 0
        return self._thisptr.id()

    ### @brief 外部入力の時に True を返す．
    @property
    def is_primary_input(self) :
        if not self.is_valid :
            return False
        return self._thisptr.is_primary_input()

    ### @brief 入力番号を返す．
    ###
    ### 入力でなければ値は不定
    @property
    def input_id(self) :
        if not self.is_valid :
            return -1
        return self._thisptr.input_id()

    ### @brief 関連するDFFを返す．
    ###
    ### DFFに関連しないノードの場合は不定
    @property
    def dff(self) :
        cdef const CXX_TpgDff* c_dff
        if not self.is_valid :
            return None
        c_dff = self._thisptr.dff()
        return to_TpgDff(c_dff)

    ### @brief ゲートタイプを返す．
    ###
    ### 論理ゲート型でない場合は不定
    @property
    def gate_type(self) :
        if self._is_valid :
            return to_GateType(self._thisptr.gate_type())
        return None

    ### @brief controling value を得る．
    @property
    def cval(self) :
        if self._is_valid :
            return to_Val3(self._thisptr.cval())
        return None

    ### @brief noncontroling value を得る．
    @property
    def nval(self) :
        if self._is_valid :
            return to_Val3(self._thisptr.nval())
        return None

    ### @brief controling output value を得る．
    @property
    def coval(self) :
        if self._is_valid :
            return to_Val3(self._thisptr.coval())
        return None

    ### @brief noncontroling output value を得る．
    @property
    def noval(self) :
        if self._is_valid :
            return to_Val3(self._thisptr.noval())
        return None

    ### @brief ファンイン数を返す．
    ###
    ### is_logic == True でなければ値は不定
    @property
    def fanin_num(self) :
        if not self.is_valid :
            return 0
        return self._thisptr.fanin_num()

    ### @brief ファンインを返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
    ###
    ### is_logic == True でなければ値は不定
    def fanin(self, pos) :
        cdef const CXX_TpgNode* c_inode
        if not self.is_valid :
            return None
        c_inode = self._thisptr.fanin(pos)
        return to_TpgNode(c_inode)

    ### @brief ファンアウト数を返す．
    @property
    def fanout_num(self) :
        if not self.is_valid :
            return 0
        return self._thisptr.fanout_num()

    ### @brief ファンアウトを返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < fanout_num() )
    def fanout(self, pos) :
        if not self.is_valid :
            return 0
        return to_TpgNode(self._thisptr.fanout(pos))

    ### @brief FFRの根のノードを返す．
    def ffr_root(self) :
        cdef const CXX_TpgNode* c_node
        if not self.is_valid :
            return None
        c_node = self._thisptr.ffr_root()
        return to_TpgNode(c_node)

    ### @brief MFFCの根のノードを返す．
    def mffc_root(self) :
        cdef const CXX_TpgNode* c_node
        if not self.is_valid :
            return None
        c_node = self._thisptr.mffc_root()
        return to_TpgNode(c_node)

    ### @brief immediate dominator を返す．
    def imm_dom(self) :
        cdef const CXX_TpgNode* c_node
        if not self._is_valid :
            return None
        c_node = self._thisptr.imm_dom()
        return to_TpgNode(c_node)


### @brief CXX_TpgNode* から TpgNode を作る関数
cdef to_TpgNode(const CXX_TpgNode* c_node) :
    if c_node == NULL :
        return None
    ans = TpgNode()
    ans._thisptr = c_node
    return ans
