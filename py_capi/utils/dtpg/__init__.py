#! /usr/bin/env python3

""" dtpg サブモジュールの初期化スクリプト

:file: __init__.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from .structengine_gen import StructEngineGen
from .dtpgresult_gen import DtpgResultGen
from .dtpgstats_gen import DtpgStatsGen
from .vidmap_gen import VidMapGen
from .dtpg_args import StructEngineArg, DtpgResultArg, DtpgStatsArg


gen_list = [
    StructEngineGen(),
    DtpgResultGen(),
    DtpgStatsGen(),
    VidMapGen(),
]
