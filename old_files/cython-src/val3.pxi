
### @file val3.pxi
### @brief val3 の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2018 Yusuke Matsunaga
### All rights reserved.

from enum import Enum
from CXX_Val3 cimport Val3 as CXX_Val3
from CXX_Val3 cimport __val3_to_int, __int_to_val3

### @brief Val3 の Python バージョン
class Val3(Enum) :
    _X = 0
    _0 = 1
    _1 = 2


### @brief C++ の Val3 を Python の Val3 に変換する
cdef to_Val3(CXX_Val3 c_val) :
    cdef int c_int = __val3_to_int(c_val)
    if c_int == 0 :
        return Val3._X
    elif c_int == 1 :
        return Val3._0
    elif c_int == 2 :
        return Val3._1

### @brief Python の Val3 を C++ の Val3 に変換する．
cdef CXX_Val3 from_Val3(val) :
    cdef int c_int
    if val == Val3._X :
        c_int = 0
    elif val == Val3._0 :
        c_int = 1
    elif val == Val3._1 :
        c_int = 2
    return __int_to_val3(c_int)
