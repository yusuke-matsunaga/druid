#
# @file CXX_DtpgResult.pxd
# @brief CXX_DtpgResult 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.


from CXX_FaultStatus cimport FaultStatus
from CXX_TestVector cimport TestVector


cdef extern from "DtpgResult.h" namespace "nsYm::nsDruid" :

    ### @brief DtpgResult の Cython バージョン
    cdef cppclass DtpgResult :
        DtpgResult()
        @staticmethod
        make_untestable()
        DtpgResult(const TestVector&)
        FaultStatus status()
        const TestVector& testvector()
