#
# @file fsim.pxi
# @brief Fsim の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


from CXX_Fsim cimport Fsim as CXX_Fsim
from CXX_TestVector cimport TestVector as CXX_TestVector
from CXX_FaultType cimport FaultType as CXX_FaultType


cdef class Fsim :
    cdef CXX_Fsim* _thisptr

    ## @brief 初期化
    def __cinit__(Fsim self, name, TpgNetwork network, fault_type) :
        cdef CXX_FaultType c_ftype = from_FaultType(fault_type)
        if name == 'Fsim2' :
            self._thisptr = CXX_Fsim.new_Fsim2(network._this, c_ftype)
        elif name == 'Fsim3' :
            self._thisptr = CXX_Fsim.new_Fsim3(network._this, c_ftype)
        else :
            assert False

    ## @brief 終了処理
    def __dealloc__(Fsim self) :
        if self._thisptr != NULL :
            del self._thisptr

    ## @brief 全故障を対象から外す．
    def set_skip_all(Fsim self) :
        self._thisptr.set_skip_all()

    ## @brief 指定した故障を対象から外す．
    def set_skip(Fsim self, TpgFault f) :
        self._thisptr.set_skip(f._thisptr)

    ## @brief 指定した故障のリストを対象から外す．
    def set_skip_list(Fsim self, f_list) :
        cdef int n = len(f_list)
        cdef vector[const CXX_TpgFault*] c_fault_list
        cdef TpgFault fault
        c_fault_list.reserve(n)
        for fault in f_list :
            c_fault_list.push_back(fault._thisptr)
        self._thisptr.set_skip(c_fault_list)

    ## @brief 全故障を対象にする．
    def clear_skip_all(Fsim self) :
        self._thisptr.clear_skip_all()

    ## @brief 指定した故障を対象に含める．
    def clear_skip(Fsim self, TpgFault f) :
        self._thisptr.clear_skip(f._thisptr)

    ## @brief 指定した故障のリストを対象に含める．
    def clear_skip_list(Fsim self, f_list) :
        cdef int n = len(f_list)
        cdef vector[const CXX_TpgFault*] c_fault_list
        cdef TpgFault fault
        c_fault_list.reserve(n)
        for fault in f_list :
            c_fault_list.push_back(fault._thisptr)
        self._thisptr.clear_skip(c_fault_list)

    ## @brief SPSFP シミュレーションを行う．
    def spsfp(Fsim self, TestVector tv, TpgFault f) :
        return self._thisptr.spsfp(tv._thisptr, f._thisptr)

    ## @brief SPSFP シミュレーションを行う．
    def spsfp(Fsim self, NodeValList assign_list, TpgFault f) :
        return self._thisptr.spsfp(assign_list._this, f._thisptr)

    ## @brief SPPFP シミュレーションを行う．
    def sppfp(Fsim self, TestVector tv) :
        return self._thisptr.sppfp(tv._thisptr)

    ## @brief SPPFP シミュレーションを行う．
    def sppfp(Fsim self, NodeValList assign_list) :
        return self._thisptr.sppfp(assign_list._this)

    ## @brief PPSFP シミュレーションを行う．
    def ppsfp(Fsim self) :
        return self._thisptr.ppsfp()

    ## @brief 遷移故障モードで信号遷移回数を数える．
    def calc_wsa(Fsim self, TestVector tv, bool weighted = False) :
        return self._thisptr.calc_wsa(tv._thisptr, weighted)

    ## @brief パタンバッファをクリアする．
    def clear_patterns(Fsim self) :
        self._thisptr.clear_patterns()

    ## @brief パタンをバッファにセットする．
    def set_pattern(Fsim self, int pos, TestVector tv) :
        self._thisptr.set_pattern(pos, tv._thisptr)

    ## @brief バッファのパタンを取り出す．
    def get_pattern(Fsim self, int pos) :
        cdef const CXX_TestVector* c_tv = self._thisptr.get_pattern(pos)
        return TestVector()

    ## @brief 直前の PPSFP で検出された故障数を返す．
    def det_fault_num(Fsim self) :
        return self._thisptr.det_fault_num()

    ## @brief 直前の PPSFP で検出された故障を得る．
    def det_fault(Fsim self, int pos) :
        cdef const CXX_TpgFault* c_fault = self._thisptr.det_fault(pos)
        return to_TpgFault(c_fault)

    ## @brief 故障を検出しているパタンのビットベクタを返す．
    def det_fault_pat(Fsim self, int pos) :
        return self._thisptr.det_fault_pat(pos)
