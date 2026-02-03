#! /usr/bin/env python3

""" PyAssignIter2 の生成スクリプト

:file: assigniter2_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class AssignIter2Gen(PyObjGen):

    def __init__(self):
        super().__init__(classname='AssignIter2',
                         pyname='AssignIter2',
                         namespace='DRUID',
                         header_include_files=['types/AssignList.h'],
                         source_include_files=['pym/PyAssignIter2.h',
                                               'pym/PyAssign.h'])

        self.add_dealloc('default')

        def iter_func(writer):
            writer.gen_return_self()
        self.add_iter(iter_func)

        def next_func(writer):
            with writer.gen_if_block('val.has_next()'):
                writer.gen_return_pyobject('PyAssign',
                                           'val.next()')
            writer.gen_return('nullptr')
        self.add_iternext(next_func)

        self.add_conv('default')
