#
# @file CXX_GateType.pxd
# @brief GateType 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "GateType.h" namespace "nsYm::nsDruid" :

    cdef enum GateType :
        CONST0,
        CONST1,
        INPUT,
        BUFF,
        NOT,
        AND,
        NAND,
        OR,
        NOR,
        XOR,
        XNOR

    cdef int __gate_type_to_int(GateType)
    cdef GateType __int_to_gate_type(int)
