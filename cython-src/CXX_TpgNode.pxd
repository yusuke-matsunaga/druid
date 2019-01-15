#
# @file CXX_TpgNode.pxd
# @brief TpgNode 用の pxd ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_TpgDff cimport TpgDff
from CXX_TpgFault cimport TpgFault
from CXX_GateType cimport GateType
from CXX_Val3 cimport Val3


cdef extern from "TpgNode.h" namespace "nsYm::nsDruid" :

    ## @brief TpgNode の cython バージョン
    cdef cppclass TpgNode :
        int id()
        bool is_primary_input()
        bool is_primary_output()
        bool is_dff_input()
        bool is_dff_output()
        bool is_dff_clock()
        bool is_dff_clear()
        bool is_dff_preset()
        bool is_ppi()
        bool is_ppo()
        bool is_logic()
        int input_id()
        int output_id()
        int output_id2()
        const TpgDff* dff()
        GateType gate_type()
        Val3 cval()
        Val3 nval()
        Val3 coval()
        Val3 noval()
        int fanin_num()
        const TpgNode* fanin(int)
        int fanout_num()
        const TpgNode* fanout(int)
        const TpgNode* ffr_root()
#        const TpgFFR* ffr()
        const TpgNode* mffc_root()
#        const TpgMFFC* mffc()
        const TpgNode* imm_dom()
#        int fault_num()
#        const TpgFault* fault(int)
