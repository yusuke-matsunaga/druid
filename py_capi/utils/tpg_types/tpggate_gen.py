#! /usr/bin/env python3

""" PyTpgGate を生成するスクリプト

:file: tpggate_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg
from .types_args import Val3Arg, Fval2Arg


class TpgGateGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgGate',
                         pyname='TpgGate',
                         namespace='DRUID',
                         header_include_files=['types/TpgGate.h'],
                         source_include_files=['pym/PyTpgGate.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PyTpgFault.h',
                                               'pym/PyVal3.h',
                                               'pym/PyFval2.h',
                                               'pym/PyPrimType.h',
                                               'pym/PyExpr.h',
                                               'pym/PyString.h',
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

        def get_name(writer):
            writer.gen_return_py_string('val.name()')
        self.add_getter('get_name',
                        func_body=get_name)
        self.add_attr('name',
                      getter_name='get_name')

        def meth_output_node(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.output_node()')
        self.add_method('output_node',
                        func_body=meth_output_node,
                        doc_str='return Output Node')

        def meth_input_num(writer):
            writer.gen_return_py_ulong('val.input_num()')
        self.add_method('input_num',
                        func_body=meth_input_num,
                        doc_str='return the number of inputs')

        def meth_input_node(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.input_node(pos)')
        self.add_method('input_node',
                        func_body=meth_input_node,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return Input Node')

        def meth_branch_info(writer):
            writer.gen_auto_assign('val1',
                                   'PyTpgNode::ToPyObject(val.branch_info(pos).node)')
            writer.gen_auto_assign('val2',
                                   'PyUlong::ToPyObject(val.branch_info(pos).ipos)')
            writer.gen_return_buildvalue('(Ok)', ['val1', 'val2'])
        self.add_method('branch_info',
                        func_body=meth_branch_info,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return BranchInfo')

        def meth_is_ppi(writer):
            writer.gen_return_py_bool('val.is_ppi()')
        self.add_method('is_ppi',
                        func_body=meth_is_ppi,
                        doc_str='True if PPI')

        def meth_is_ppo(writer):
            writer.gen_return_py_bool('val.is_ppo()')
        self.add_method('is_ppo',
                        func_body=meth_is_ppo,
                        doc_str='True if PPO')

        def meth_is_simple(writer):
            writer.gen_return_py_bool('val.is_simple()')
        self.add_method('is_simple',
                        func_body=meth_is_simple,
                        doc_str='True if simple type')

        def meth_is_complex(writer):
            writer.gen_return_py_bool('val.is_complex()')
        self.add_method('is_complex',
                        func_body=meth_is_complex,
                        doc_str='True if complex type')

        def get_primitive_type(writer):
            writer.gen_return_pyobject('PyPrimType',
                                       'val.primitive_type()')
        self.add_getter('get_primitive_type',
                        func_body=get_primitive_type)
        self.add_attr('primitive_type',
                      getter_name='get_primitive_type')

        def get_expr(writer):
            writer.gen_return_pyobject('PyExpr',
                                       'val.expr()')
        self.add_getter('get_expr',
                        func_body=get_expr)
        self.add_attr('expr',
                      getter_name='get_expr')

        def get_extra_node_num(writer):
            writer.gen_return_py_ulong('val.extra_node_num()')
        self.add_getter('get_extra_node_num',
                        func_body=get_extra_node_num)
        self.add_attr('extra_node_num',
                      getter_name='get_extra_node_num')

        def meth_cval(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.cval(ipos, ival)')
        self.add_method('cval',
                        func_body=meth_cval,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='ipos'),
                                  Val3Arg(name='val',
                                          cvarname='ival')],
                        doc_str='return the controlling value')

        def meth_stem_fault(writer):
            writer.gen_return_pyobject('PyTpgFault',
                                       'val.stem_fault(fval)')
        self.add_method('stem_fault',
                        func_body=meth_stem_fault,
                        arg_list=[Fval2Arg(name='fval',
                                           cvarname='fval')],
                        doc_str='return Stem Fault')

        def meth_branch_fault(writer):
            writer.gen_return_pyobject('PyTpgFault',
                                       'val.branch_fault(ipos, fval)')
        self.add_method('branch_fault',
                        func_body=meth_branch_fault,
                        arg_list=[UlongArg(name='ipos',
                                          cvarname='ipos'),
                                  Fval2Arg(name='fval',
                                           cvarname='fval')],
                        doc_str='return Stem Fault')

        self.add_conv('default')
