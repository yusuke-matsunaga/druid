
### @file CXX_FaultStatusMgr.pxd
### @brief CXX_FaultStatusMgr 用の pxd ファイル
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from CXX_TpgNetwork cimport TpgNetwork
from CXX_TpgFault cimport TpgFault
from CXX_FaultStatus cimport FaultStatus


cdef extern from "FaultStatusMgr.h" namespace "nsYm::nsDruid" :

    ### @brief FaultStatusMgr の Cython バージョン
    cdef cppclass FaultStatusMgr :
        FaultStatusMgr(const TpgNetwork& network)
        void set(const TpgFault* fault, FaultStatus status)
        FaultStatus get(const TpgFault* fault)
