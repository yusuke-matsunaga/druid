#
# @file CXX_FaultStatus.pxd
# @brief FaultStatus 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "FaultStatus.h" namespace "nsYm::nsDruid" :

    cdef enum FaultStatus :
        Undetected
        Detected
        Untestable

    cdef int __fault_status_to_int(FaultStatus)
    cdef FaultStatus __int_to_fault_status(int)
