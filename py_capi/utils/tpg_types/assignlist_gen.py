#! /usr/bin/env python3

""" PyAssignList を生成するスクリプト

:file: assignlist_gen.py
:author: Yusuke Matsunaga (松永 裕介)n
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from .types_args import AssignArg, AssignListArg

class AssignListGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='AssignList',
                         pyname='AssignList',
                         namespace='DRUID',
                         header_include_files=['types/AssignList.h'],
                         source_include_files=['pym/PyAssignList.h',
                                               'pym/PyAssignIter2.h',
                                               'pym/PyAssign.h',
                                               'pym/PyUlong.h'])

        def meth_clear(writer):
            writer.gen_stmt('val.clear()')
            writer.gen_return_py_none()
        self.add_method('clear',
                        func_body=meth_clear,
                        doc_str='clear')

        def meth_add(writer):
            writer.gen_stmt('val.add(assign)')
            writer.gen_return_py_none()
        self.add_method('add',
                        func_body=meth_add,
                        arg_list=[AssignArg(name='assign',
                                            cvarname='assign')],
                        doc_str=['add Assign',
                                 ':param Assign assign: ノードの値'])

        def meth_merge(writer):
            writer.gen_stmt('val.merge(src_list)')
            writer.gen_return_py_none()
        self.add_method('merge',
                        func_body=meth_merge,
                        arg_list=[AssignListArg(name='src_list',
                                                cvarname='src_list')],
                        doc_str=['merge AssignList',
                                 ':param AssignList src_list: マージ対象のリスト'])

        def sq_length(writer):
            writer.gen_auto_assign('len_val', 'val.size()')
        def sq_item(writer):
            writer.gen_return_pyobject('PyAssign',
                                       'val.elem(index)')
        self.add_sequence(sq_length=sq_length,
                          sq_item=sq_item)

        def iter_func(writer):
            writer.gen_return_pyobject('PyAssignIter2',
                                       'val.iter()')
        self.add_iter(iter_func)

        self.add_conv('default')

        self.add_deconv('default')
