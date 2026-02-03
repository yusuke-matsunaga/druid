#! /usr/bin/env python3

""" PyTpgFaultIter2 を生成するスクリプト

:file: tpgfaultiter2_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class TpgFaultIter2Gen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgFaultIter2',
                         pyname='TpgFaultIter2',
                         namespace='DRUID',
                         header_include_files=['types/TpgFaultList.h'],
                         source_include_files=['pym/PyTpgFaultIter2.h',
                                               'pym/PyTpgFault.h'])

        self.add_dealloc('default')

        def iter_func(writer):
            writer.gen_return_self()
        self.add_iter(iter_func)

        def next_func(writer):
            with writer.gen_if_block('val.has_next()'):
                writer.gen_return_pyobject('PyTpgFault',
                                           'val.next()')
            writer.gen_return('nullptr')
        self.add_iternext(next_func)

        self.add_conv('default')
