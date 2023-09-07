
### @file CXX_DtpgStats.pxd
### @brief DtpgStats 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.


from CXX_USTime cimport USTime
from CXX_SatStats cimport SatStats


cdef extern from "DtpgStats.h" namespace "nsYm::nsDruid" :

    ### @brief DtpgStats の Cython バージョン
    cdef cppclass DtpgStats :
        DtpgStats()
        void clear()
        void update_det(const SatStats&, const USTime&)
        void update_red(const SatStats&, const USTime&)
        void update_abort(const SatStats&, const USTime&)
        void merge(const DtpgStats&)
