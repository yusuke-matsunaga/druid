#! /usr/bin/env python3

""" PyDetCond を生成するスクリプト

:file: detcond_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class DetCondGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='DetCond',
                         pyname='DetCond',
                         namespace='DRUID',
                         header_include_files=['condgen/DetCond.h'],
                         source_include_files=['pym/PyDetCond.h'])

        self.add_dealloc('default')

        self.add_conv('default')
        self.add_deconv('default')
