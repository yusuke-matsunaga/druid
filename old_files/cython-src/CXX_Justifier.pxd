
### @file CXX_Justifier.pxd
### @brief CXX_Justifier 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.


cdef extern from "Justifier.h" namespace "nsYm::nsDruid" :

    ### @brief Justifier の Cython バージョン
    cdef cppclass Justifier :
        pass

    ### @brief Just1 を作る．
    cdef Justifier* new_Just1(int)

    ### @brief Just2 を作る．
    cdef Justifier* new_Just2(int)
