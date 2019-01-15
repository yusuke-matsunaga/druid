#
# @file CXX_Fsim.pxd
# @brief Fsim 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


from libcpp cimport bool
from libcpp.vector cimport vector
from CXX_TpgNetwork cimport TpgNetwork
from CXX_TpgFault cimport TpgFault
from CXX_TestVector cimport TestVector
from CXX_NodeValList cimport NodeValList
from CXX_FaultType cimport FaultType

ctypedef unsigned long PackedVal


cdef extern from "Fsim.h" namespace "nsYm::nsDruid" :

    ## @brief Fsim の cython バージョン
    cdef cppclass Fsim :
        Fsim()
        void init_fsim2(const TpgNetwork& network, FaultType ftype)
        void init_fsim3(const TpgNetwork& network, FaultType ftype)
        void set_skip_all()
        void set_skip(const TpgFault* f)
        void set_skip(const vector[const TpgFault*]& fault_list)
        void clear_skip_all()
        void clear_skip(const TpgFault* f)
        void clear_skip(const vector[const TpgFault*]& fault_list)
        bool spsfp(const TestVector& tv, const TpgFault* f)
        bool spsfp(const NodeValList& assign_list, const TpgFault* f)
        int sppfp(const TestVector& tv)
        int sppfp(const NodeValList& assign_list)
        int ppsfp()
        int calc_wsa(const TestVector& tv, bool weighted)
        void clear_patterns()
        void set_pattern(int pos, const TestVector& tv)
        TestVector get_pattern(int pos)
        int det_fault_num()
        const TpgFault* det_fault(int pos)
        PackedVal det_fault_pat(int pos)
