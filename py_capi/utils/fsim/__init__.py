#! /usr/bin/env python3

""" fsim サブモジュールの初期化スクリプト

:file: __init__.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from .diffbits_gen import DiffBitsGen
from .fsim_gen import FsimGen
from .fsimresults_gen import FsimResultsGen
from .fsim_args import DiffBitsArg
from .fsim_args import FsimResultsArg


gen_list = [
    DiffBitsGen(),
    FsimGen(),
    FsimResultsGen(),
]
