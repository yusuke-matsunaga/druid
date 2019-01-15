#
# @file CXX_TpgDff.pxd
# @brief TpgDff 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from CXX_TpgNode cimport TpgNode


cdef extern from "TpgDff.h" namespace "nsYm::nsDruid" :

    ## @brief TpgDff の Cython バージョン
    cdef cppclass TpgDff :
        int id()
        const TpgNode* input()
        const TpgNode* output()
        const TpgNode* clock()
        const TpgNode* clear()
        const TpgNode* preset()
