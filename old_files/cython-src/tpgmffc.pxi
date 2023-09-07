
### @file tpgmffc.pxi
### @brief tpgmffc の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_TpgMFFC cimport TpgMFFC as CXX_TpgMFFC
from CXX_TpgFault cimport TpgFault as CXX_TpgFault


### @brief TpgMFFC の Python バージョン
cdef class TpgMFFC :
    cdef const CXX_TpgMFFC* _thisptr

    @property
    def fault_num(self) :
        return self._thisptr.fault_num()

    def fault_list(self) :
        cdef int n = self._thisptr.fault_num()
        cdef int i
        cdef const CXX_TpgFault* c_fault
        for i in range(0, n) :
            c_fault = self._thisptr.fault(i)
            yield to_TpgFault(c_fault)


cdef to_TpgMFFC(const CXX_TpgMFFC& c_mffc) :
    ans = TpgMFFC()
    ans._thisptr = &c_mffc
    return ans
