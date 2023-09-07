
### @file CXX_DtpgEngine.pxd
### @brief DtpgEngine 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.


from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from CXX_iostream cimport ostream
from CXX_FaultType cimport FaultType
from CXX_TpgNetwork cimport TpgNetwork
from CXX_TpgNode cimport TpgNode
from CXX_TpgFFR cimport TpgFFR
from CXX_TpgMFFC cimport TpgMFFC
from CXX_TpgFault cimport TpgFault
from CXX_DtpgResult cimport DtpgResult
from CXX_DtpgStats cimport DtpgStats
from CXX_SatBool3 cimport SatBool3
from CXX_SatSolverType cimport SatSolverType
from CXX_TestVector cimport TestVector

cdef extern from "DtpgFFR.h" namespace "nsYm::nsDruid" :

    ## @brief DtpgFFR の Cython バージョン
    cdef cppclass DtpgFFR :
        DtpgFFR(const TpgNetwork&, FaultType, const TpgFFR&, const string&, const SatSolverType)
        DtpgResult gen_pattern(const TpgFault*)
        DtpgResult gen_k_patterns(const TpgFault*, int, vector[TestVector])
        const DtpgStats& stats()


cdef extern from "DtpgMFFC.h" namespace "nsYm::nsDruid" :

    ## @brief DtpgMFFC の Cython バージョン
    cdef cppclass DtpgMFFC :
        DtpgMFFC(const TpgNetwork&, FaultType, const TpgMFFC&, const string&, const SatSolverType)
        DtpgResult gen_pattern(const TpgFault*)
        const DtpgStats& stats()
