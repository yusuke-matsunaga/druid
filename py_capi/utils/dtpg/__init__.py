#! /usr/bin/env python3

""" dtpg サブモジュールの初期化スクリプト

:file: __init__.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from .structengine_gen import StructEngineGen
from .bdengine_gen import BdEngineGen
from .dtpgmgr_gen import DtpgMgrGen
from .dtpgresults_gen import DtpgResultsGen
from .vidmap_gen import VidMapGen
from .dtpg_args import StructEngineArg, DtpgResultsArg, DtpgStatsArg


gen_list = [
    StructEngineGen(),
    BdEngineGen(),
    DtpgMgrGen(),
    DtpgResultsGen(),
    VidMapGen(),
]
