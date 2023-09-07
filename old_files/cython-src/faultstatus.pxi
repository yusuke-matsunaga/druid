
### @file faultstatus.pxi
### @brief FaultStatus の Python インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.

from enum import Enum
from CXX_FaultStatus cimport FaultStatus as CXX_FaultStatus
from CXX_FaultStatus cimport __fault_status_to_int, __int_to_fault_status


### @brief FaultStatus の Python バージョン
class FaultStatus(Enum) :
    Undetected = 0
    Detected = 1
    Untestable = 2

    ### @brief 内容を表す文字列を返す．
    def __str__(self) :
        if self == FaultStatus.Undetected :
            return 'undetected'
        if self == FaultStatus.Detected :
            return 'detected'
        if self == FaultStatus.Untestable :
            return 'untestable'
        assert False

### @brief C++ の FaultStatus から変換する
cdef to_FaultStatus(CXX_FaultStatus c_status) :
    cdef int val = __fault_status_to_int(c_status)
    if val == 0 : return FaultStatus.Undetected
    if val == 1 : return FaultStatus.Detected
    if val == 2 : return FaultStatus.Untestable
    return None

### @brief C++ の FaultStatus に変換する
cdef CXX_FaultStatus from_FaultStatus(status) :
    cdef int val
    if status == FaultStatus.Undetected :
        val = 0
    elif status == FaultStatus.Detected :
        val = 1
    elif status == FaultStatus.Untestable :
        val = 2
    else :
        assert False
    return __int_to_fault_status(val)
