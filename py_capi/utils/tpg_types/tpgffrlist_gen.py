#! /usr/bin/env python3

""" PyTpgFFRList を生成するスクリプト

:file: tpgffrlist_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class TpgFFRListGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgFFRList',
                         pyname='TpgFFRList',
                         namespace='DRUID',
                         header_include_files=['types/TpgFFRList.h'],
                         source_include_files=['pym/PyTpgFFRList.h',
                                               'pym/PyTpgFFRIter2.h',
                                               'pym/PyTpgFFR.h',
                                               'pym/PyUlong.h'])

        self.add_dealloc('default')

        def meth_is_valid(writer):
            writer.gen_return_py_bool('val.is_valid()')
        self.add_method('is_valid',
                        func_body=meth_is_valid,
                        doc_str='True if valid')

        def sq_length(writer):
            writer.gen_auto_assign('len_val', 'val.size()')
        def sq_item(writer):
            writer.gen_return_pyobject('PyTpgFFR',
                                       'val[index]')
        self.add_sequence(sq_length=sq_length,
                          sq_item=sq_item)

        def iter_func(writer):
            writer.gen_return_pyobject('PyTpgFFRIter2',
                                       'val.iter()')
        self.add_iter(iter_func)

        self.add_conv('default')
