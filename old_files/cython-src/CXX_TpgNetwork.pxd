### @file CXX_TpgNetwork.pxd
### @brief TpgNetwork 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from CXX_ClibCellLibrary cimport ClibCellLibrary
from CXX_TpgNode cimport TpgNode
from CXX_TpgMFFC cimport TpgMFFC
from CXX_TpgFFR cimport TpgFFR
from CXX_TpgDff cimport TpgDff
from CXX_TpgFault cimport TpgFault


cdef extern from "TpgNetwork.h" namespace "nsYm::nsDruid" :

    ### @brief TpgNetwork の cython バージョン
    cdef cppclass TpgNetwork :
        TpgNetwork()
        bool read_blif(const string& filename)
        bool read_blif(const string& filename, const ClibCellLibrary& cell_library)
        bool read_iscas89(const string& filename)
        int node_num()
        const TpgNode* node(int)
        int input_num()
        const TpgNode* input(int)
        int output_num()
        const TpgNode* output(int)
        const TpgNode* output2(int)
        int ppi_num()
        const TpgNode* ppi(int)
        int ppo_num()
        const TpgNode* ppo(int)
        int mffc_num()
        const TpgMFFC& mffc(int)
        int ffr_num()
        const TpgFFR& ffr(int)
        int dff_num()
        const TpgDff& dff(int)
        int max_fault_id()
        int rep_fault_num()
        const TpgFault* rep_fault(int pos)
        int node_rep_fault_num(int)
        const TpgFault* node_rep_fault(int, int)
