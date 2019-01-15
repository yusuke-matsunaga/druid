
### @file CXX_TpgFault.pxd
### @brief TpgFault 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_TpgNode cimport TpgNode


cdef extern from "TpgFault.h" namespace "nsYm::nsDruid" :

    ## @brief TpgFault の Cython バージョン
    cdef cppclass TpgFault :
        int id()
        const TpgNode* tpg_inode()
        const TpgNode* tpg_onode()
        bool is_stem_fault()
        bool is_branch_fault()
        int fault_pos()
        int tpg_pos()
        int val()
        string str()
        bool is_rep()
        const TpgFault* rep_fault()
