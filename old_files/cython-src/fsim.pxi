
### @file fsim.pxi
### @brief Fsim の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.


from CXX_Fsim cimport Fsim as CXX_Fsim
from CXX_TestVector cimport TestVector as CXX_TestVector
from CXX_FaultType cimport FaultType as CXX_FaultType


cdef class Fsim :
    cdef CXX_Fsim _this

    ### @brief 初期化
    ### @param[in] name シミュレータの種類 ('Fsim2' or 'Fsim3')
    ### @param[in] network 対象のネットワーク
    ### @param[in] fault_type 故障の種類
    def __cinit__(Fsim self, name, TpgNetwork network, fault_type) :
        cdef CXX_FaultType c_ftype = from_FaultType(fault_type)
        if name == 'Fsim2' :
            self._this.init_fsim2(network._this, c_ftype)
        elif name == 'Fsim3' :
            self._this.init_fsim3(network._this, c_ftype)
        else :
            assert False

    ### @brief 全故障を対象から外す．
    def set_skip_all(Fsim self) :
        self._this.set_skip_all()

    ### @brief 指定した故障を対象から外す．
    ### @param[in] f 故障
    def set_skip(Fsim self, TpgFault f) :
        self._this.set_skip(f._thisptr)

    ### @brief 指定した故障のリストを対象から外す．
    def set_skip_list(Fsim self, f_list) :
        cdef int n = len(f_list)
        cdef vector[const CXX_TpgFault*] c_fault_list
        cdef TpgFault fault
        c_fault_list.reserve(n)
        for fault in f_list :
            c_fault_list.push_back(fault._thisptr)
        self._this.set_skip(c_fault_list)

    ### @brief 全故障を対象にする．
    def clear_skip_all(Fsim self) :
        self._this.clear_skip_all()

    ### @brief 指定した故障を対象に含める．
    ### @param[in] f 故障
    def clear_skip(Fsim self, TpgFault f) :
        self._this.clear_skip(f._thisptr)

    ### @brief 指定した故障のリストを対象に含める．
    ### @param[in] f_list 故障のリスト
    def clear_skip_list(Fsim self, f_list) :
        cdef int n = len(f_list)
        cdef vector[const CXX_TpgFault*] c_fault_list
        cdef TpgFault fault
        c_fault_list.reserve(n)
        for fault in f_list :
            c_fault_list.push_back(fault._thisptr)
        self._this.clear_skip(c_fault_list)

    ### @brief SPSFP シミュレーションを行う．
    ### @param[in] tv テストベクタ
    ### @param[in] f 対象の故障
    def spsfp(Fsim self, TestVector tv, TpgFault f) :
        return self._this.spsfp(tv._this, f._thisptr)

    ### @brief SPSFP シミュレーションを行う．
    ### @param[in] assign_list 値の割当リスト
    def spsfp(Fsim self, NodeValList assign_list, TpgFault f) :
        return self._this.spsfp(assign_list._this, f._thisptr)

    ### @brief SPPFP シミュレーションを行う．
    ### @param[in] tv テストベクタ
    def sppfp(Fsim self, TestVector tv) :
        cdef int n_det = self._this.sppfp(tv._this)
        cdef const CXX_TpgFault* c_fault
        fault_list = []
        for i in range(n_det) :
            c_fault = self._this.det_fault(i)
            fault_list.append(to_TpgFault(c_fault))
        return fault_list

    ### @brief SPPFP シミュレーションを行う．
    ### @param[in] assign_list 値の割当リスト
    def sppfp(Fsim self, NodeValList assign_list) :
        cdef int n_det = self._this.sppfp(assign_list._this)
        cdef const CXX_TpgFault* c_fault
        fault_list = []
        for i in range(n_det) :
            c_fault = self._this.det_fault(i)
            fault_list.append(to_TpgFault(c_fault))
        return fault_list

    ### @brief PPSFP シミュレーションを行う．
    ### @param[in] tv_list テストベクタのリスト
    ### @return 検出された故障と検出したパタン番号のリストのリスト
    ###
    ### パタン番号は tv_list 中の位置
    def ppsfp(Fsim self, tv_list) :
        cdef int n_det
        cdef TestVector tv
        cdef const CXX_TpgFault* c_fault
        cdef unsigned long c_pat
        cdef TpgFault fault
        self._this.clear_patterns()
        pos = 0
        for tv in tv_list :
            self._this.set_pattern(pos, tv._this)
            pos += 1
        n_det = self._this.ppsfp()
        fault_patid_list = []
        for i in range(n_det) :
            c_fault = self._this.det_fault(i)
            c_pat = self._this.det_fault_pat(i)
            fault = to_TpgFault(c_fault)
            patid_list = []
            for j in range(pos) :
                if c_pat & (1 << j) :
                    patid_list.append(j)
            fault_patid_list.append( (fault, patid_list) )
        return fault_patid_list

    ### @brief 遷移故障モードで信号遷移回数を数える．
    def calc_wsa(Fsim self, TestVector tv, bool weighted = False) :
        return self._this.calc_wsa(tv._this, weighted)
