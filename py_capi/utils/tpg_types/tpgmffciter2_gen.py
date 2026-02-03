#! /usr/bin/env python3

""" PyTpgMFFCIter2 を生成するスクリプト

:file: tpgmffciter2_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class TpgMFFCIter2Gen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgMFFCIter2',
                         pyname='TpgMFFCIter2',
                         namespace='DRUID',
                         header_include_files=['types/TpgMFFCList.h'],
                         source_include_files=['pym/PyTpgMFFCIter2.h',
                                               'pym/PyTpgMFFC.h'])

        self.add_dealloc('default')

        def iter_func(writer):
            writer.gen_return_self()
        self.add_iter(iter_func)

        def next_func(writer):
            with writer.gen_if_block('val.has_next()'):
                writer.gen_return_pyobject('PyTpgMFFC',
                                           'val.next()')
            writer.gen_return('nullptr')
        self.add_iternext(next_func)

        self.add_conv('default')
