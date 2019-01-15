
### @file CXX_MinPatMgr.pxd
### @brief CXX_MinPatMgr 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from CXX_TpgFault cimport TpgFault
from CXX_TestVector cimport TestVector
from CXX_TpgNetwork cimport TpgNetwork
from CXX_FaultType cimport FaultType

cdef extern from "MinPatMgr.h" namespace "nsYm::nsDruid" :

    ### @brief MinPatMgr の cython バージョン
    cdef cppclass MinPatMgr :
        MinPatMgr()
        @staticmethod
        void fault_reduction(const vector[const TpgFault*]& fault_list,
                             const TpgNetwork& network,
                             FaultType fault_type,
                             const string& red_algorithm,
                             bool debug)
        @staticmethod
        void gen_mcsets(const vector[TestVector]& tv_list,
                        vector[TestVector]& new_tv_list)
        @staticmethod
        int coloring(const vector[const TpgFault*]& fault_list,
                     const vector[TestVector]& tv_list,
                     const TpgNetwork& network,
                     FaultType fault_type,
                     vector[TestVector]& new_tv_list)
