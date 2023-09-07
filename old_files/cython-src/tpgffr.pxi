
### @file tpgffr.pxi
### @brief tpgffr の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_TpgFFR cimport TpgFFR as CXX_TpgFFR


### @brief TpgFFR の Python バージョン
cdef class TpgFFR :
    cdef const CXX_TpgFFR* _thisptr

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


cdef to_TpgFFR(const CXX_TpgFFR& c_ffr) :
    ans = TpgFFR()
    ans._thisptr = &c_ffr
    return ans
