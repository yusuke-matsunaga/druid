#! /usr/bin/env python3

""" PyAssign を生成するスクリプト

:file: assign_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class AssignGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='Assign',
                         pyname='Assign',
                         namespace='DRUID',
                         header_include_files=['types/Assign.h'],
                         source_include_files=['pym/PyAssign.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PyInt.h',
                                               'pym/PyUlong.h',
                                               'pym/PyBool.h'])

        self.add_dealloc('default')

        def meth_node_id(writer):
            writer.gen_return_py_ulong('val.node_id()')
        self.add_method('node_id',
                        func_body=meth_node_id,
                        doc_str='return node ID')

        def meth_node(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.node()')
        self.add_method('node',
                        func_body=meth_node,
                        doc_str='return node')

        def meth_time(writer):
            writer.gen_return_py_int('val.time()')
        self.add_method('time',
                        func_body=meth_time,
                        doc_str='return time')

        def meth_node_time(writer):
            writer.gen_return_py_ulong('val.node_time()')
        self.add_method('node_time',
                        func_body=meth_node_time,
                        doc_str='return the packed value of node_id and time')

        def meth_val(writer):
            writer.gen_return_py_bool('val.val()')
        self.add_method('val',
                        func_body=meth_val,
                        doc_str='return value')

        def hash_func(writer):
            writer.gen_return('val.hash()')
        self.add_hash(hash_func)

        self.add_richcompare('cmp_default')

        self.add_conv('default')
        self.add_deconv('default')
