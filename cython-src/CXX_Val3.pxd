#
# @file CXX_Val3.pxd
# @brief Val3 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool

cdef extern from "Val3.h" namespace "nsYm::nsDruid" :

    cdef enum Val3 :
        _X,
        _0,
        _1

    cdef int __val3_to_int(Val3)
    cdef Val3 __int_to_val3(int)
