
### @file faultstatusmgr.pxi
### @brief faultstatusmgr の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_FaultStatusMgr cimport FaultStatusMgr as CXX_FaultStatusMgr
from CXX_FaultStatus cimport FaultStatus as CXX_FaultStatus


### @brief FaultStatusMgr の Python バージョン
cdef class FaultStatusMgr :
    cdef CXX_FaultStatusMgr* _thisptr

    ### @brief 初期化
    def __cinit__(FaultStatusMgr self, TpgNetwork network) :
        self._thisptr = new CXX_FaultStatusMgr(network._this)

    ### @brief 終了処理
    def __dealloc__(FaultStatusMgr self) :
        if self._thisptr != NULL :
            del self._thisptr

    ### @brief 故障の状態を設定する．
    def set(FaultStatusMgr self, TpgFault fault, status) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef CXX_FaultStatus c_status = from_FaultStatus(status)
        self._thisptr.set(c_fault, c_status)

    ### @brief 故障の状態を得る．
    def get(FaultStatusMgr self, TpgFault fault) :
        cdef const CXX_TpgFault* c_fault = from_TpgFault(fault)
        cdef CXX_FaultStatus c_status = self._thisptr.get(c_fault)
        return to_FaultStatus(c_status)
