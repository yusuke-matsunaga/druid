
### @file tpgnetwork.pyx
### @brief TpgNetwork の python インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2016, 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_TpgNetwork cimport TpgNetwork as CXX_TpgNetwork
from CXX_TpgNode cimport TpgNode as CXX_TpgNode
from CXX_TpgMFFC cimport TpgMFFC as CXX_TpgMFFC
from CXX_TpgFFR cimport TpgFFR as CXX_TpgFFR


### @brief TpgNetwork の Python ヴァージョン
cdef class TpgNetwork :
    cdef CXX_TpgNetwork _this

    ### @brief blif ファイルを読み込む．
    ### @param[in] filename ファイル名
    ### @param[in] cell_library セルライブラリ
    ### @return 結果のネットワークを返す．
    ###
    ### エラーが起きたら None を返す．
    @staticmethod
    def read_blif(str filename, CellLibrary cell_library = None) :
        cdef string c_filename = filename.encode('UTF-8')
        cdef bool stat
        network = TpgNetwork()
        if cell_library == None :
            stat = network._this.read_blif(c_filename)
        else :
            stat = network._this.read_blif(c_filename, cell_library._this)
        if stat :
            return network
        else :
            return None

    ### @brief iscas89(.bench)ファイルを読み込む
    ### @param[in] filename ファイル名
    ### @return 結果のネットワークを返す．
    ###
    ### エラーが起きたら None を返す．
    @staticmethod
    def read_iscas89(str filename) :
        cdef string c_filename = filename.encode('UTF-8')
        cdef bool stat
        network = TpgNetwork()
        stat = network._this.read_iscas89(c_filename)
        if stat :
            return network
        else :
            return None

    ### @brief ノード数を返す．
    @property
    def node_num(TpgNetwork self) :
        return self._this.node_num()

    ### @brief ID番号に対応したノードを返す．
    ### @param[in] id ID番号 ( 0 <= id < node_num )
    def node_by_id(TpgNetwork self, id) :
        cdef const CXX_TpgNode* c_node = self._this.node(id)
        return to_TpgNode(c_node)

    ### @brief 全ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def node_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n = self._this.node_num()
        cdef int i
        for i in range(0, n) :
            c_node = self._this.node(i)
            yield to_TpgNode(c_node)

    ### @brief 外部入力のノード数を返す．
    @property
    def input_num(TpgNetwork self) :
        return self._this.input_num()

    ### @brief 外部入力番号から外部入力ノードを得る．
    ### @param[in] id 外部入力番号 ( 0 <= id < input_num )
    def input_by_id(TpgNetwork self, id) :
        cdef const CXX_TpgNode* c_node = self._this.input(id)
        return to_TpgNode(c_node)

    ### @brief 外部入力ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def input_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n = self._this.input_num()
        cdef int i
        for i in range(0, n) :
            c_node = self._this.input(i)
            yield to_TpgNode(c_node)

    ### @brief 外部出力のノード数を返す．
    @property
    def output_num(TpgNetwork self) :
        return self._this.output_num()

    ### @brief 外部出力番号から外部出力ノードを返す．
    ### @param[in] id 外部出力番号 ( 0 <= id < output_num )
    def output_by_id(TpgNetwork self, id) :
        cdef const CXX_TpgNode* c_node = self._this.output(id)
        return to_TpgNode(c_node)

    ### @brief 外部出力ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def output_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n = self._this.output_num()
        cdef int i
        for i in range(0, n) :
            c_node = self._this.output(i)
            yield to_TpgNode(c_node)

    ### @brief TFIのサイズの降順に並べた外部出力ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def sorted_output_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n = self._this.output_num()
        cdef int i
        for i in range(0, n) :
            c_node = self._this.output2(i)
            yield to_TpgNode(c_node)

    ### @brief 疑似外部入力数を返す．
    @property
    def ppi_num(TpgNetwork self) :
        return self._this.ppi_num()

    ### @brief ID番号から擬似外部入力ノードを返す．
    ### @param[in] id ID番号 ( 0 <= id < ppi_num )
    def ppi_by_id(TpgNetwork self, id) :
        cdef const CXX_TpgNode* c_node = self._this.ppi(id)
        return to_TpgNode(c_node)

    ### @brief 疑似外部入力ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def ppi_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n = self._this.ppi_num()
        cdef int i
        for i in range(0, n) :
            c_node = self._this.ppi(i)
            yield to_TpgNode(c_node)

    ### @brief DFF出力ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def dff_output_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n1 = self._this.input_num()
        cdef int n2 = self._this.ppi_num()
        cdef int i
        for i in range(n1, n2) :
            c_node = self._this.ppi(i)
            yield to_TpgNode(c_node)

    ### @brief 疑似外部出力数を返す．
    @property
    def ppo_num(TpgNetwork self) :
        return self._this.ppo_num()

    ### @brief ID番号から擬似外部出力ノードを返す．
    ### @param[in] id ID番号 ( 0 <= id < ppi_num )
    def ppo_by_id(TpgNetwork self, id) :
        cdef const CXX_TpgNode* c_node = self._this.ppo(id)
        return to_TpgNode(c_node)

    ### @brief 疑似外部出力ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def ppo_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n = self._this.ppo_num()
        cdef int i
        for i in range(0, n) :
            c_node = self._this.ppo(i)
            yield to_TpgNode(c_node)

    ### @brief MFFC 数を返す．
    @property
    def mffc_num(TpgNetwork self) :
        return self._this.mffc_num()

    ### @brief MFFC のリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def mffc_list(TpgNetwork self) :
        cdef int n = self._this.mffc_num()
        cdef int i
        for i in range(0, n) :
            yield to_TpgMFFC(self._this.mffc(i))

    ### @brief FFR 数を返す．
    @property
    def ffr_num(TpgNetwork self) :
        return self._this.ffr_num()

    ### @brief FFR のリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def ffr_list(TpgNetwork self) :
        cdef int n = self._this.ffr_num()
        cdef int i
        for i in range(0, n) :
            yield to_TpgFFR(self._this.ffr(i))

    ### @brief DFF入力ノードのリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def dff_input_list(TpgNetwork self) :
        cdef const CXX_TpgNode* c_node
        cdef int n1 = self._this.input_num()
        cdef int n2 = self._this.ppo_num()
        cdef int i
        for i in range(n1, n2) :
            c_node = self._this.ppo(i)
            yield to_TpgNode(c_node)

    ### @brief 故障番号の最大値(+1)を返す．
    @property
    def max_fault_id(TpgNetwork self) :
        return self._this.max_fault_id()

    ### @brief 代表故障数を返す．
    @property
    def rep_fault_num(TpgNetwork self) :
        return self._this.rep_fault_num()

    ### @brief 代表故障のリストを返す．
    ###
    ### 正確にはジェネレータを返す．
    def rep_fault_list(TpgNetwork self) :
        cdef const CXX_TpgFault* c_fault
        cdef int n = self._this.rep_fault_num()
        cdef int i
        for i in range(0, n) :
            c_fault = self._this.rep_fault(i)
            yield to_TpgFault(c_fault)

    ### @brief ノードに関係した代表故障を返す．
    ### @param[in] node_id ノード番号
    def node_rep_fault_num(TpgNetwork self, int node_id) :
        return self._this.node_rep_fault_num(node_id)

    ### @brief ノードに関係した代表故障のリストを返す．
    ### @param[in] node_id ノード番号
    ###
    ### 正確にはジェネレータを返す．
    def node_rep_fault_list(TpgNetwork self, int node_id) :
        cdef const CXX_TpgFault* c_fault
        cdef int n = self._this.node_rep_fault_num(node_id)
        cdef int i
        for i in range(0, n) :
            c_fault = self._this.node_rep_fault(node_id, i)
            yield to_TpgFault(c_fault)
