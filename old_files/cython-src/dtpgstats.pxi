
### @file dtpgstats.pxi
### @brief dtpgstats の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_DtpgStats cimport DtpgStats as CXX_DtpgStats


### @brief DtpgStats の Python バージョン
cdef class DtpgStats :
    cdef CXX_DtpgStats _this

    ### @brief クリアする
    def clear(DtpgStats self) :
        self._this.clear()


### @brief C++ の DtpgStats から Python の DtpgStats に変換する．
cdef to_DtpgStats(CXX_DtpgStats c_stats) :
    ans = DtpgStats()
    ans._this = c_stats
    return ans
