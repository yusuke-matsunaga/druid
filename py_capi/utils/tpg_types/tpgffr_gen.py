#! /usr/bin/env python3

""" PyTpgFFR を生成するスクリプト

:file: tpgffr_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg


class TpgFFRGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgFFR',
                         pyname='TpgFFR',
                         namespace='DRUID',
                         header_include_files=['types/TpgFFR.h'],
                         source_include_files=['pym/PyTpgFFR.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PyTpgNodeList.h',
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

        def get_input_num(writer):
            writer.gen_return_py_ulong('val.input_num()')
        self.add_getter('get_input_num',
                        func_body=get_input_num)
        self.add_attr('input_num',
                      getter_name='get_input_num')

        def meth_input(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.input(pos)')
        self.add_method('input',
                        func_body=meth_input,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return input node')

        def meth_input_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.input_list()')
        self.add_method('input_list',
                        func_body=meth_input_list,
                        doc_str='return list of input nodes')

        def get_node_num(writer):
            writer.gen_return_py_ulong('val.node_num()')
        self.add_getter('get_node_num',
                        func_body=get_node_num)
        self.add_attr('node_num',
                      getter_name='get_node_num')

        def meth_node(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.node(pos)')
        self.add_method('node',
                        func_body=meth_node,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return node')

        def meth_node_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.node_list()')
        self.add_method('node_list',
                        func_body=meth_node_list,
                        doc_str='return list of nodes')

        self.add_conv('default')
