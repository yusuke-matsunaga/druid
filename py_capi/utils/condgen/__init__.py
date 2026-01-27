#! /usr/bin/env python3

""" condgen サブモジュールの初期化スクリプト

:file: __init__.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from .detcond_gen import DetCondGen
from .condgenmgr_gen import CondGenMgrGen
from .condgenstats_gen import CondGenStatsGen
from .condgen_args import DetCondArg, DetCondListArg
from .condgen_args import CondGenMgrArg
from .condgen_args import CondGenStatsArg


gen_list = [
    DetCondGen(),
    CondGenMgrGen(),
    CondGenStatsGen(),
]
