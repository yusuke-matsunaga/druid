
### @file dtpgengine.pxi
### @brief DtpgEngine の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp.pair cimport pair
from libcpp.vector cimport vector
from CXX_DtpgEngine cimport DtpgFFR as CXX_DtpgFFR
from CXX_DtpgEngine cimport DtpgMFFC as CXX_DtpgMFFC
from CXX_TpgFault cimport TpgFault as CXX_TpgFault
from CXX_NodeValList cimport NodeValList as CXX_NodeValList
from CXX_SatBool3 cimport SatBool3 as CXX_SatBool3
from CXX_DtpgResult cimport DtpgResult as CXX_DtpgResult
from cython.operator cimport dereference as deref

### @brief DtpgFFR の Python バージョン
cdef class DtpgFFR :
    cdef CXX_DtpgFFR* _thisptr

    ### @brief 初期化
    def __cinit__(DtpgFFR self, TpgNetwork network, fault_type, TpgFFR ffr,
                  **kwargs) :
        cdef CXX_FaultType c_ftype = from_FaultType(fault_type)
        cdef SatSolverType solver_type = kwargs.get('solver_type', SatSolverType())
        cdef string c_jt = kwargs.get('just_type', 'Just2').encode('UTF-8')
        self._thisptr = new CXX_DtpgFFR(network._this, c_ftype, deref(ffr._thisptr), c_jt,
                                        solver_type._this)

    ### @brief 終了処理
    def __dealloc__(DtpgFFR self) :
        if self._thisptr != NULL :
            del self._thisptr

    ### @brief パタン生成を行う．
    def __call__(DtpgFFR self, TpgFault fault, k = 1) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef CXX_DtpgResult c_result
        cdef CXX_TestVector c_tv
        cdef vector[CXX_TestVector] c_tv_list
        if k > 1 :
            c_result = self._thisptr.gen_k_patterns(c_fault, k, c_tv_list)
            return to_FaultStatus(c_result.status()), [ to_TestVector(c_tv) for c_tv in c_tv_list ]
        else :
            c_result = self._thisptr.gen_pattern(c_fault)
            return to_FaultStatus(c_result.status()), to_TestVector(c_result.testvector())

    ### @brief 統計情報を得る．
    @property
    def stats(DtpgFFR self) :
        cdef CXX_DtpgStats c_stats = self._thisptr.stats()
        return to_DtpgStats(c_stats)


### @brief DtpgMFFC の Python バージョン
cdef class DtpgMFFC :
    cdef CXX_DtpgMFFC* _thisptr

    ### @brief 初期化
    def __cinit__(DtpgMFFC self, TpgNetwork network, fault_type, TpgMFFC mffc,
                  **kwargs) :
        cdef CXX_FaultType c_ftype = from_FaultType(fault_type)
        cdef SatSolverType solver_type = kwargs.get('solver_type', SatSolverType())
        cdef string c_jt = kwargs.get('just_type', 'Just2').encode('UTF-8')
        self._thisptr = new CXX_DtpgMFFC(network._this, c_ftype, deref(mffc._thisptr), c_jt,
                                         solver_type._this)

    ### @brief 終了処理
    def __dealloc__(DtpgMFFC self) :
        if self._thisptr != NULL :
            del self._thisptr

    ### @brief パタン生成を行う．
    def __call__(DtpgMFFC self, TpgFault fault) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef CXX_DtpgResult c_result = self._thisptr.gen_pattern(c_fault)
        return to_FaultStatus(c_result.status()), to_TestVector(c_result.testvector())

    ### @brief 統計情報を得る．
    @property
    def stats(DtpgMFFC self) :
        cdef CXX_DtpgStats c_stats = self._thisptr.stats()
        return to_DtpgStats(c_stats)
