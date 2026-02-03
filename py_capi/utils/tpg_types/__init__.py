#! /usr/bin/env python3

""" types サブモジュールの初期化スクリプト

:file: __init__.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from .val3_gen import Val3Gen
from .fval2_gen import Fval2Gen
from .faulttype_gen import FaultTypeGen
from .faultstatus_arg import FaultStatusGen
from .tpgnetwork_gen import TpgNetworkGen
from .tpgnode_gen import TpgNodeGen
from .tpgnodelist_gen import TpgNodeListGen
from .tpgnodeiter2_gen import TpgNodeIter2Gen
from .tpgmffc_gen import TpgMFFCGen
from .tpgmffclist_gen import TpgMFFCListGen
from .tpgmffciter2_gen import TpgMFFCIter2Gen
from .tpgffr_gen import TpgFFRGen
from .tpgffrlist_gen import TpgFFRListGen
from .tpgffriter2_gen import TpgFFRIter2Gen
from .tpggate_gen import TpgGateGen
from .tpggatelist_gen import TpgGateListGen
from .tpggateiter2_gen import TpgGateIter2Gen
from .tpgfault_gen import TpgFaultGen
from .tpgfaultlist_gen import TpgFaultListGen
from .tpgfaultiter2_gen import TpgFaultIter2Gen
from .testvector_gen import TestVectorGen
from .inputvector_gen import InputVectorGen
from .assign_gen import AssignGen
from .assignlist_gen import AssignListGen
from .assigniter2_gen import AssignIter2Gen
from .types_args import Val3Arg, FaultTypeArg, FaultStatusArg
from .types_args import TpgNetworkRefArg, TpgNodeArg, TpgNodeListArg
from .types_args import TpgFaultArg, TpgFaultListArg
from .types_args import TestVectorArg, TestVectorListArg
from .types_args import InputVectorArg
from .types_args import AssignArg, AssignListArg


gen_list = [
    Val3Gen(),
    Fval2Gen(),
    FaultTypeGen(),
    FaultStatusGen(),
    TpgNetworkGen(),
    TpgNodeGen(),
    TpgNodeListGen(),
    TpgNodeIter2Gen(),
    TpgMFFCGen(),
    TpgMFFCListGen(),
    TpgMFFCIter2Gen(),
    TpgFFRGen(),
    TpgFFRListGen(),
    TpgFFRIter2Gen(),
    TpgGateGen(),
    TpgGateListGen(),
    TpgGateIter2Gen(),
    TpgFaultGen(),
    TpgFaultListGen(),
    TpgFaultIter2Gen(),
    TestVectorGen(),
    InputVectorGen(),
    AssignGen(),
    AssignListGen(),
    AssignIter2Gen(),
]
