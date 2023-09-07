
### @file CXX_ColCov.pxd
### @brief CXX_ColCov 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.


from libcpp.string cimport string
from libcpp.pair cimport pair
from libcpp.vector cimport vector

cdef extern from "ColCov.h" namespace "nsYm::nsDruid" :

    ### @brief ColCov の Cython バージョン
    cdef cppclass ColCov :
        ColCov()
        ColCov(int, int)
        void resize(int, int)
        void insert_elem(int, int)
        void insert_conflict(int, int)
        int row_size()
        int col_size()
        const vector[pair[int, int]]& elem_list()
        const vector[pair[int, int]]& conflict_list()
        int heuristic(const string&, const string&, vector[int]&)
