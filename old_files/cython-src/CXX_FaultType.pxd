#
# @file CXX_FaultType.pxd
# @brief FaultType 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "FaultType.h" namespace "nsYm::nsDruid" :

    cdef enum FaultType :
        None,
        StuckAt,
        TransitionDelay

    cdef int __fault_type_to_int(FaultType)
    cdef FaultType __int_to_fault_type(int)
