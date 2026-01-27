#! /usr/bin/env python3

""" PyTpgFault を生成するスクリプト

:file: tpgfault_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class TpgFaultGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgFault',
                         pyname='TpgFault',
                         namespace='DRUID',
                         header_include_files=['types/TpgFault.h'],
                         source_include_files=['pym/PyTpgFault.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PyTpgGate.h',
                                               'pym/PyFaultType.h',
                                               'pym/PyFval2.h',
                                               'pym/PyAssignList.h',
                                               'pym/PyList.h',
                                               'pym/PyString.h',
                                               'pym/PyLong.h',
                                               'pym/PyBoolList.h',
                                               'pym/PyBool.h'])

        self.add_dealloc('default')

        def meth_is_valid(writer):
            writer.gen_return_py_bool('val.is_valid()')
        self.add_method('is_valid',
                        func_body=meth_is_valid,
                        doc_str='True if valid')

        def get_id(writer):
            writer.gen_return_py_long('val.id()')
        self.add_getter('get_id',
                        func_body=get_id)
        self.add_attr('id',
                      getter_name='get_id')

        def get_gate(writer):
            writer.gen_return_pyobject('PyTpgGate',
                                       'val.gate()')
        self.add_getter('get_gate',
                        func_body=get_gate)
        self.add_attr('gate',
                      getter_name='get_gate')

        def get_fault_type(writer):
            writer.gen_return_pyobject('PyFaultType',
                                       'val.fault_type()')
        self.add_getter('get_fault_type',
                        func_body=get_fault_type)
        self.add_attr('fault_type',
                      getter_name='get_fault_type')

        def get_fval(writer):
            writer.gen_return_pyobject('PyFval2',
                                       'val.fval()')
        self.add_getter('get_fval',
                        func_body=get_fval)
        self.add_attr('fval',
                      getter_name='get_fval')

        def meth_is_stem(writer):
            writer.gen_return_py_bool('val.is_stem()')
        self.add_method('is_stem',
                        func_body=meth_is_stem,
                        doc_str='True if stem fault')

        def meth_is_branch(writer):
            writer.gen_return_py_bool('val.is_branch()')
        self.add_method('is_branch',
                        func_body=meth_is_branch,
                        doc_str='True if branch fault')

        def get_branch_pos(writer):
            writer.gen_return_py_long('val.branch_pos()')
        self.add_getter('get_branch_pos',
                        func_body=get_branch_pos)
        self.add_attr('branch_pos',
                      getter_name='get_branch_pos')

        def get_input_vals(writer):
            writer.gen_return_pyobject('PyBoolList',
                                       'val.input_vals()')
        self.add_getter('get_input_vals',
                        func_body=get_input_vals)
        self.add_attr('input_vals',
                      getter_name='get_input_vals')

        def get_rep_fault(writer):
            writer.gen_return_pyobject('PyTpgFault',
                                       'val.rep_fault()')
        self.add_getter('get_rep_fault',
                        func_body=get_rep_fault)
        self.add_attr('rep_fault',
                      getter_name='get_rep_fault')

        def get_origin_node(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.origin_node()')
        self.add_getter('get_origin_node',
                        func_body=get_origin_node)
        self.add_attr('origin_node',
                      getter_name='get_origin_node')

        def get_excitation_condition(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.excitation_condition()')
        self.add_getter('get_excitation_condition',
                        func_body=get_excitation_condition)
        self.add_attr('excitation_condition',
                      getter_name='get_excitation_condition')

        def get_ffr_root(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.ffr_root()')
        self.add_getter('get_ffr_root',
                        func_body=get_ffr_root)
        self.add_attr('ffr_root',
                      getter_name='get_ffr_root')

        def get_ffr_propagate_condition(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.ffr_propagate_condition()')
        self.add_getter('get_ffr_propagate_condition',
                        func_body=get_ffr_propagate_condition)
        self.add_attr('ffr_propagate_condition',
                      getter_name='get_ffr_propagate_condition')

        def hash_func(writer):
            writer.gen_return('val.hash()')
        self.add_hash(hash_func)

        self.add_richcompare('eq_default')

        def str_body(writer):
            writer.gen_return_py_string('val.str()')
        self.add_str(str_body)

        self.add_conv('default')

        self.add_deconv('default')
