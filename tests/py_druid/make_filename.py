#! /usr/bin/env python3

"""ファイル名を作る関数

:file: make_filename.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import os


def make_filename(basename):
    TESTDATA_DIR = os.environ.get('TESTDATA_DIR')
    filename = os.path.join(TESTDATA_DIR, basename)
    return filename
    
