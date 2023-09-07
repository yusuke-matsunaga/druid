
### @file CXX_TestVector.pxd
### @brief TestVector 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_FaultType cimport FaultType
from CXX_Val3 cimport Val3


cdef extern from "TestVector.h" namespace "nsYm::nsDruid" :

    ### @brief TestVector の cython ヴァージョン
    cdef cppclass TestVector :
        TestVector()
        TestVector(int, int, FaultType)
        int vector_size()
        Val3 val(int pos)
        int input_num()
        int dff_num()
        bool has_aux_input()
        int ppi_num()
        FaultType fault_type()
        Val3 ppi_val(int pos)
        Val3 input_val(int pos)
        Val3 dff_val(int pos)
        Val3 aux_input_val(int pos)
        int x_count()
        string bin_str()
        string hex_str()
        void merge(TestVector& right)
        void init()
        void set_ppi_val(int pos, Val3 val)
        void set_input_val(int pos, Val3 val)
        void set_dff_val(int pos, Val3 val)
        void set_aux_input_val(int pos, Val3 val)

    bool is_compatible(const TestVector& left, const TestVector& right)
    bool is_equal(const TestVector& left, const TestVector& right)
    bool operator<(const TestVector& left, const TestVector& right)
    bool operator<=(const TestVector& left, const TestVector& right)
