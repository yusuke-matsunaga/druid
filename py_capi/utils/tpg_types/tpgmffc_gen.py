#! /usr/bin/env python3

""" PyTpgMFFC を生成するスクリプト

:file: tpgmffc_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg


class TpgMFFCGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgMFFC',
                         pyname='TpgMFFC',
                         namespace='DRUID',
                         header_include_files=['types/TpgMFFC.h'],
                         source_include_files=['pym/PyTpgMFFC.h',
                                               'pym/PyTpgFFR.h',
                                               'pym/PyTpgFFRList.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PyUlong.h'])

        self.add_dealloc('default')

        def meth_is_valid(writer):
            writer.gen_return_py_bool('val.is_valid()')
        self.add_method('is_valid',
                        func_body=meth_is_valid,
                        doc_str='True if valid')

        def get_id(writer):
            writer.gen_return_py_ulong('val.id()')
        self.add_getter('get_id',
                        func_body=get_id)
        self.add_attr('id',
                      getter_name='get_id')

        def meth_root(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.root()')
        self.add_method('root',
                        func_body=meth_root,
                        doc_str='return root node')

        def meth_ffr_num(writer):
            writer.gen_return_py_ulong('val.ffr_num()')
        self.add_method('ffr_num',
                        func_body=meth_ffr_num,
                        doc_str='return the number of FFRs')

        def meth_ffr(writer):
            writer.gen_return_pyobject('PyTpgFFR',
                                       'val.ffr(pos)')
        self.add_method('ffr',
                        func_body=meth_ffr,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return FFR')

        def meth_ffr_list(writer):
            writer.gen_return_pyobject('PyTpgFFRList',
                                       'val.ffr_list()')
        self.add_method('ffr_list',
                        func_body=meth_ffr_list,
                        doc_str='return list of FFRs')

        self.add_conv('default')
