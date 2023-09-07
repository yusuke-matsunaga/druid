
### @file gate_type.pxi
### @brief GateType の Python インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from enum import Enum
from CXX_GateType cimport GateType as CXX_GateType
from CXX_GateType cimport __gate_type_to_int, __int_to_gate_type


### @brief GateType の Python バージョン
class GateType(Enum) :
    CONST0 = 0
    CONST1 = 1
    INPUT  = 2
    BUFF   = 3
    NOT    = 4
    AND    = 5
    NAND   = 6
    OR     = 7
    NOR    = 8
    XOR    = 9
    XNOR   = 10


### @brief C++ の GateType を Python の GateType に変換する．
cdef to_GateType(CXX_GateType c_gate_type) :
    cdef c_val = __gate_type_to_int(c_gate_type)
    if c_val == 0 :
        return GateType.CONST0
    elif c_val == 1:
        return GateType.CONST1
    elif c_val == 2:
        return GateType.INPUT
    elif c_val == 3:
        return GateType.BUFF
    elif c_val == 4:
        return GateType.NOT
    elif c_val == 5:
        return GateType.AND
    elif c_val == 6:
        return GateType.NAND
    elif c_val == 7:
        return GateType.OR
    elif c_val == 8:
        return GateType.NOR
    elif c_val == 9:
        return GateType.XOR
    elif c_val == 10:
        return GateType.XNOR


### @brief Python の GateType を C++ の GateType に変換する．
cdef CXX_GateType from_GateType(val) :
    cdef int c_val = 0
    if val == GateType.CONST0 :
        c_val = 0
    elif val == GateType.CONST1 :
        c_val = 1
    elif val == GateType.INPUT :
        c_val = 2
    elif val == GateType.BUFF :
        c_val = 3
    elif val == GateType.NOT :
        c_val = 4
    elif val == GateType.AND :
        c_val = 5
    elif val == GateType.NAND :
        c_val = 6
    elif val == GateType.OR :
        c_val = 7
    elif val == GateType.NOR :
        c_val = 8
    elif val == GateType.XOR :
        c_val = 9
    elif val == GateType.XNOR :
        c_val = 10
    return __int_to_gate_type(c_val)
