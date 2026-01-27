#! /usr/bin/env python3

""" PyCondGenMgr を生成するスクリプト

:file: condgenmgr_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class CondGenMgrGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='CondGenMgr',
                         pyname='CondGenMgr',
                         namespace='DRUID',
                         header_include_files=['condgen/CondGenMgr.h'],
                         source_include_files=['pym/PyCondGenMgr.h'])

        self.add_dealloc('default')
