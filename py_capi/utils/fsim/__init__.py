#! /usr/bin/env python3

""" fsim サブモジュールの初期化スクリプト

:file: __init__.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from .diffbits_gen import DiffBitsGen
from .diffbitsarray_gen import DiffBitsArrayGen
from .fsim_gen import FsimGen
from .fsim_args import DiffBitsArg, DiffBitsArrayArg


gen_list = [
    DiffBitsGen(),
    DiffBitsArrayGen(),
    FsimGen(),
]
