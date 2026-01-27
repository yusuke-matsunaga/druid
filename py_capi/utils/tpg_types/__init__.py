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
from .tpgmffc_gen import TpgMFFCGen
from .tpgmffclist_gen import TpgMFFCListGen
from .tpgffr_gen import TpgFFRGen
from .tpgffrlist_gen import TpgFFRListGen
from .tpggate_gen import TpgGateGen
from .tpggatelist_gen import TpgGateListGen
from .tpgfault_gen import TpgFaultGen
from .tpgfaultlist_gen import TpgFaultListGen
from .testvector_gen import TestVectorGen
from .inputvector_gen import InputVectorGen
from .assign_gen import AssignGen
from .assignlist_gen import AssignListGen
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
    TpgMFFCGen(),
    TpgMFFCListGen(),
    TpgFFRGen(),
    TpgFFRListGen(),
    TpgGateGen(),
    TpgGateListGen(),
    TpgFaultGen(),
    TpgFaultListGen(),
    TestVectorGen(),
    InputVectorGen(),
    AssignGen(),
    AssignListGen(),
]
