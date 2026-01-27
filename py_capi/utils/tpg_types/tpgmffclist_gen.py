#! /usr/bin/env python3

""" PyTpgMFFCList を生成するスクリプト

:file: tpgmffclist_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class TpgMFFCListGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgMFFCList',
                         pyname='TpgMFFCList',
                         namespace='DRUID',
                         header_include_files=['types/TpgMFFCList.h'],
                         source_include_files=['pym/PyTpgMFFCList.h',
                                               'pym/PyTpgMFFC.h',
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
            writer.gen_return_pyobject('PyTpgMFFC',
                                       'val[index]')
        self.add_sequence(sq_length=sq_length,
                          sq_item=sq_item)

        self.add_conv('default')
