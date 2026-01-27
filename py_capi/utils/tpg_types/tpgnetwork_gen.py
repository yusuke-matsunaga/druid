#! /usr/bin/env python3

""" PyTpgNetwork を生成するスクリプト

:file: tpgnetwork_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import StringArg, UlongArg, RawObjArg
from bn import BnModelArg
from .types_args import FaultTypeArg


class TpgNetworkGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgNetwork',
                         pyname='TpgNetwork',
                         namespace='DRUID',
                         header_include_files=['types/TpgNetwork.h'],
                         source_include_files=['pym/PyTpgNetwork.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PyTpgNodeList.h',
                                               'pym/PyTpgFault.h',
                                               'pym/PyTpgFaultList.h',
                                               'pym/PyTpgMFFC.h',
                                               'pym/PyTpgMFFCList.h',
                                               'pym/PyTpgFFR.h',
                                               'pym/PyTpgFFRList.h',
                                               'pym/PyTpgGate.h',
                                               'pym/PyTpgGateList.h',
                                               'pym/PyBnModel.h',
                                               'pym/PyFaultType.h',
                                               'pym/pyfstream.h',
                                               'pym/PyUlong.h'])

        self.add_new('default')
        self.add_dealloc('default')

        def meth_from_bn(writer):
            writer.gen_return_pyobject('PyTpgNetwork',
                                       'TpgNetwork::from_bn(model, fault_type)')
        self.add_static_method('from_bn',
                               func_body=meth_from_bn,
                               arg_list=[BnModelArg(name='model',
                                                    cvarname='model'),
                                         FaultTypeArg(name='fault_type',
                                                      cvarname='fault_type')],
                               doc_str='convert from BnModel')

        def meth_read_blif(writer):
            writer.gen_return_pyobject('PyTpgNetwork',
                                       'TpgNetwork::read_blif(filename, fault_type)')
        self.add_static_method('read_blif',
                               func_body=meth_read_blif,
                               arg_list=[StringArg(name='filename',
                                                   cvarname='filename'),
                                         FaultTypeArg(name='fault_type',
                                                      cvarname='fault_type')],
                               doc_str='read BLIF file')

        def meth_is_valid(writer):
            writer.gen_return_py_bool('val.is_valid()')
        self.add_method('is_valid',
                        func_body=meth_is_valid,
                        doc_str='True if valid')

        def meth_node_num(writer):
            writer.gen_return_py_ulong('val.node_num()')
        self.add_method('node_num',
                        func_body=meth_node_num,
                        doc_str='return the number of nodes')

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

        def meth_input_num(writer):
            writer.gen_return_py_ulong('val.input_num()')
        self.add_method('input_num',
                        func_body=meth_input_num,
                        doc_str='return the number input nodes')

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
                        func_body=meth_node_list,
                        doc_str='return list of input nodes')

        def meth_output_num(writer):
            writer.gen_return_py_ulong('val.output_num()')
        self.add_method('output_num',
                        func_body=meth_output_num,
                        doc_str='return the number output nodes')

        def meth_output(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.output(pos)')
        self.add_method('output',
                        func_body=meth_output,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return output node')

        def meth_output_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.output_list()')
        self.add_method('output_list',
                        func_body=meth_node_list,
                        doc_str='return list of output nodes')

        def meth_output2(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.output2(pos)')
        self.add_method('output2',
                        func_body=meth_output2,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return output node with another order')

        def meth_ppi(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.ppi(pos)')
        self.add_method('ppi',
                        func_body=meth_ppi,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return ppi node')

        def meth_ppi_name(writer):
            writer.gen_return_py_string('val.ppi_name(pos)')
        self.add_method('ppi_name',
                        func_body=meth_ppi,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return ppi name')

        def meth_ppi_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.ppi_list()')
        self.add_method('ppi_list',
                        func_body=meth_node_list,
                        doc_str='return list of ppi nodes')

        def meth_ppo_num(writer):
            writer.gen_return_py_ulong('val.ppo_num()')
        self.add_method('ppo_num',
                        func_body=meth_ppo_num,
                        doc_str='return the number ppo nodes')

        def meth_ppo(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.ppo(pos)')
        self.add_method('ppo',
                        func_body=meth_ppo,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return ppo node')

        def meth_ppo_name(writer):
            writer.gen_return_py_string('val.ppo_name(pos)')
        self.add_method('ppo_name',
                        func_body=meth_ppo,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return ppo name')

        def meth_ppo_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.ppo_list()')
        self.add_method('ppo_list',
                        func_body=meth_node_list,
                        doc_str='return list of ppo nodes')

        def meth_mffc_num(writer):
            writer.gen_return_py_ulong('val.mffc_num()')
        self.add_method('mffc_num',
                        func_body=meth_mffc_num,
                        doc_str='return the number mffc nodes')

        def meth_mffc(writer):
            writer.gen_return_pyobject('PyTpgMFFC',
                                       'val.mffc(pos)')
        self.add_method('mffc',
                        func_body=meth_mffc,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return mffc node')

        def meth_mffc_list(writer):
            writer.gen_return_pyobject('PyTpgMFFCList',
                                       'val.mffc_list()')
        self.add_method('mffc_list',
                        func_body=meth_node_list,
                        doc_str='return list of MFFCs')

        def meth_ffr_num(writer):
            writer.gen_return_py_ulong('val.ffr_num()')
        self.add_method('ffr_num',
                        func_body=meth_ffr_num,
                        doc_str='return the number ffr nodes')

        def meth_ffr(writer):
            writer.gen_return_pyobject('PyTpgFFR',
                                       'val.ffr(pos)')
        self.add_method('ffr',
                        func_body=meth_ffr,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return ffr node')

        def meth_ffr_list(writer):
            writer.gen_return_pyobject('PyTpgFFRList',
                                       'val.ffr_list()')
        self.add_method('ffr_list',
                        func_body=meth_node_list,
                        doc_str='return list of FFRs')

        def meth_dff_num(writer):
            writer.gen_return_py_ulong('val.dff_num()')
        self.add_method('dff_num',
                        func_body=meth_dff_num,
                        doc_str='return the number dff nodes')

        def meth_dff_input(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.dff_input(pos)')
        self.add_method('dff_input',
                        func_body=meth_dff_input,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return DFF input node')

        def meth_dff_output(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.dff_output(pos)')
        self.add_method('dff_output',
                        func_body=meth_dff_output,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return DFF output node')

        def meth_gate_num(writer):
            writer.gen_return_py_ulong('val.gate_num()')
        self.add_method('gate_num',
                        func_body=meth_gate_num,
                        doc_str='return the number gate nodes')

        def meth_gate(writer):
            writer.gen_return_pyobject('PyTpgGate',
                                       'val.gate(pos)')
        self.add_method('gate',
                        func_body=meth_gate,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return gate')

        def meth_gate_list(writer):
            writer.gen_return_pyobject('PyTpgGateList',
                                       'val.gate_list()')
        self.add_method('gate_list',
                        func_body=meth_node_list,
                        doc_str='return list of gates')

        def meth_fault_type(writer):
            writer.gen_return_pyobject('PyFaultType',
                                       'val.fault_type()')
        self.add_method('fault_type',
                        func_body=meth_fault_type,
                        doc_str='return fault type')

        def meth_has_prev_state(writer):
            writer.gen_return_py_bool('val.has_prev_state()')
        self.add_method('has_prev_state',
                        func_body=meth_has_prev_state,
                        doc_str='True if having the previous state')

        def meth_rep_fault_list(writer):
            writer.gen_return_pyobject('PyTpgFaultList',
                                       'val.rep_fault_list()')
        self.add_method('rep_fault_list',
                        func_body=meth_node_list,
                        doc_str='return list of representative faults')

        def meth_max_fault_id(writer):
            writer.gen_return_py_ulong('val.max_fault_id()')
        self.add_method('max_fault_id',
                        func_body=meth_max_fault_id,
                        doc_str='return the MAX fault id')

        def meth_fault(writer):
            writer.gen_return_pyobject('PyTpgFault',
                                       'val.fault(pos)')
        self.add_method('fault',
                        func_body=meth_fault,
                        arg_list=[UlongArg(name='fault_id',
                                           cvarname='pos')],
                        doc_str='return fault')

        def meth_print(writer):
            writer.gen_auto_assign('s', 'opyfstream(fout_obj)')
            writer.gen_stmt('val.print(s)')
            writer.gen_return_py_none()
        self.add_method('print',
                        func_body=meth_print,
                        arg_list=[RawObjArg(name='fout',
                                            cvarname='fout_obj')],
                        doc_str='print')

        self.add_conv('default', move=True)
