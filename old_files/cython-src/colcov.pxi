
### @file colcov.pxi
### @brief colcov の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_ColCov cimport ColCov as CXX_ColCov
from libcpp cimport string
from libcpp.pair cimport pair
from libcpp.vector cimport vector


### @brief ColCov の Python バージョン
cdef class ColCov :
    cdef CXX_ColCov _this

    ### @brief 初期化
    def __init__(ColCov self, *args) :
        if len(args) == 0 :
            pass
        elif len(args) == 2 and type(args[0]) == int and type(args[1]) == int :
            self._this.resize(args[0], args[1])
        else :
            assert False

    ### @brief 行列の要素を追加する．
    def insert_elem(ColCov self, row_pos, col_pos) :
        self._this.insert_elem(row_pos, col_pos)

    ### @brief 列の衝突関係を追加する．
    def insert_conflict(ColCov self, col_pos1, col_pos2) :
        self._this.insert_conflict(col_pos1, col_pos2)

    ### @brief 行数を得る．
    @property
    def row_size(ColCov self) :
        return self._this.row_size()

    ### @brief 列数を得る．
    @property
    def col_size(ColCov self) :
        return self._this.col_size()

    ### @brief 行列の要素のリストを得る．
    def elem_list(ColCov self) :
        cdef vector[pair[int, int]] c_elem_list = self._this.elem_list()
        cdef pair[int, int] c_elem_pair
        for c_elem_pair in c_elem_list :
            yield c_elem_pair.first, c_elem_pair.second

    ### @brief ヒューリスティックで解を求める．
    def heuristic(ColCov self, **kwargs) :
        cdef string c_algorithm
        cdef string c_option
        cdef vector[int] c_color_map
        cdef int i
        cdef int nc
        if 'algorithm' in kwargs :
            c_algorithm = kwargs['algorithm'].encode('UTF-8')
        if 'option' in kwargs :
            c_option = kwargs['option'].encode('UTF-8')
        nc = self._this.heuristic(c_algorithm, c_option, c_color_map)
        return nc, [ c_color_map[i] for i in range(self.col_size) ]
