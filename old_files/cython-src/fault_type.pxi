
### @file fault_type.pxi
### @brief FaultType の Python インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from enum import Enum
from CXX_FaultType cimport FaultType as CXX_FaultType
from CXX_FaultType cimport __fault_type_to_int, __int_to_fault_type

### @brief FaultType の Python バージョン
class FaultType(Enum) :
    StuckAt = 0
    TransitionDelay = 1


### @brief C++ の FaultType を Python の FaultType に変換する．
cdef to_FaultType(CXX_FaultType c_val) :
    cdef int c_int = __fault_type_to_int(c_val)
    if c_int == 0:
        return FaultType.StuckAt
    elif c_int == 1:
        return FaultType.TransitionDelay
    else :
        return None

### @brief Python の FaultType を int に変換する．
cdef CXX_FaultType from_FaultType(val) :
    cdef int c_int
    if val == FaultType.StuckAt :
        c_int = 1
    elif val == FaultType.TransitionDelay :
        c_int = 2
    else :
        c_int = 0
    return __int_to_fault_type(c_int)
