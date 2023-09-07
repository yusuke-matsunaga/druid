
### @file testvector.pxi
### @brief testvector の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from CXX_TestVector cimport TestVector as CXX_TestVector
from CXX_TestVector cimport is_compatible as c_is_compatible
from CXX_FaultType cimport FaultType as CXX_FaultType


### @brief TestVector の Python バージョン
cdef class TestVector :
    cdef CXX_TestVector _this

    ### @brief 初期化
    ### @param[in] args 可変引数
    ###
    ### 引数の形は以下の通り
    ### - () 空
    ### - (TestVector src)
    ### - (int input_num)
    ### - (int input_num, int dff_num, FaultType fault_type)
    def __init__(TestVector self, *args) :
        cdef TestVector src
        cdef CXX_FaultType c_fault_type
        if len(args) == 0 :
            pass
        elif len(args) == 1 :
            if type(args[0]) == TestVector :
                src = args[0]
                self._this = src._this
            elif type(args[0]) == int :
                self._this = CXX_TestVector(int(args[0]), 0, from_FaultType(FaultType.StuckAt))
            else :
                assert False
        elif len(args) == 3 :
            if type(args[0]) == int and type(args[1]) == int and type(args[2]) == FaultType :
                c_fault_type = from_FaultType(args[2])
                self._this = CXX_TestVector(int(args[0]), int(args[1]), c_fault_type)
            else :
                assert False
        else :
            assert False

    ### @brief ベクタ長を返す．
    @property
    def vector_size(TestVector self) :
        return self._this.vector_size()

    ### @brief 値を返す．
    def val(TestVector self, pos) :
        return to_Val3(self._this.val(pos))

    ### @brief 入力数を返す．
    @property
    def input_num(TestVector self) :
        return self._this.input_num()

    ### @brief DFF数を返す．
    @property
    def dff_num(TestVector self) :
        return self._this.dff_num()

    ### @brief 2時刻目の外部入力を持つ時 true を返す．
    @property
    def has_aux_input(TestVector self) :
        return self._this.has_aux_input()

    ### @brief 疑似入力数を返す．
    @property
    def ppi_num(TestVector self) :
        return self._this.ppi_num()

    ### @brief 故障の種類を返す．
    @property
    def fault_type(TestVector self) :
        return to_FaultType(self._this.fault_type())

    ### @brief 疑似入力番号から対応した値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < ppi_num )
    ###
    ### 縮退モードの時に用いられる．
    def ppi_val(TestVector self, pos) :
        return to_Val3(self._this.ppi_val(pos))

    ### @brief 入力番号から対応した値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < input_num() )
    ###
    ### 遷移故障モードの時に用いられる．
    def input_val(TestVector self, pos) :
        return to_Val3(self._this.input_val(pos))

    ### @brief DFF番号から対応した値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
    ###
    ### 遷移故障モードの時に用いられる．
    def dff_val(TestVector self, pos) :
        return to_Val3(self._this.dff_val(pos))

    ### @brief 入力番号から対応した２時刻目の値を返す．
    ### @param[in] pos 位置番号 ( 0 <= pos < input_num() )
    ###
    ### 遷移故障モードの時に用いられる．
    def aux_input_val(TestVector self, pos) :
        return to_Val3(self._this.aux_input_val(pos))

    ### @brief X値の個数を返す．
    @property
    def x_count(TestVector self) :
        return self._this.x_count()

    ### @brief 2進表記の文字列を返す．
    @property
    def bin_str(TestVector self) :
        cdef string c_str = self._this.bin_str()
        return c_str.decode('UTF-8')

    ### @brief 16進表記の文字列を返す．
    @property
    def hex_str(TestVector self) :
        cdef string c_str = self._this.hex_str()
        return c_str.decode('UTF-8')

    ### @brief 全て未定値(X)で初期化する．
    def init(TestVector self) :
        self._this.init()

    ### @brief PPIに値を設定する．
    ### @param[in] pos PPIの位置番号
    ### @param[in] val 値
    def set_ppi_val(TestVector self, pos, val) :
        cdef CXX_Val3 c_val = from_Val3(val)
        self._this.set_ppi_val(pos, c_val)

    ### @brief 1時刻目の外部入力の値を設定する．
    ### @param[in] pos 入力の位置番号
    ### @param[in] val 値
    def set_input_val(TestVector self, pos, val) :
        cdef CXX_Val3 c_val = from_Val3(val)
        self._this.set_input_val(pos, c_val)

    ### @brief 1時刻目のDFFの値を設定する．
    ### @param[in] pos DFFの位置番号
    ### @param[in] val 値
    def set_dff_val(TestVector self, pos, val) :
        cdef CXX_Val3 c_val = from_Val3(val)
        self._this.set_dff_val(pos, c_val)

    ### @brief 2時刻目の外部入力の値を設定する．
    ### @param[in] pos 入力の位置番号
    ### @param[in] val 値
    def set_aux_input_val(TestVector self, pos, val) :
        cdef CXX_Val3 c_val = from_Val3(val)
        self._this.set_aux_input_val(pos, c_val)

    ### @brief 両立関係演算子
    @staticmethod
    def is_compatible(TestVector left, TestVector right) :
        return c_is_compatible(left._this, right._this)

    ### @brief マージ演算子
    def __and__(TestVector self, TestVector other) :
        cdef TestVector ans = TestVector(self)
        ans._this.merge(other._this)
        return ans

    ### @brief マージ演算子
    def __iand__(TestVector self, TestVector other) :
        self._this.merge(other._this)
        return self

    ### @brief 複数オペランドのマージ演算子
    @staticmethod
    def merge(tvlist) :
        n = len(tvlist)
        if n == 0 :
            return TestVector()
        elif n == 1 :
            return tvlist[0]
        else :
            tv0 = TestVector(tvlist[0])
            for i in range(1, n) :
                tv1 = tvlist[i]
                tv0 &= tv1
            return tv0

### @brief C++ の TestVector から変換する．
cdef to_TestVector(CXX_TestVector c_tv) :
    cdef TestVector tv = TestVector()
    tv._this = c_tv
    return tv
