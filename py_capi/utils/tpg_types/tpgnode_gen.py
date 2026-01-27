#! /usr/bin/env python3

""" PyTpgNode を生成するスクリプト

:file: tpgnode_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import LongArg


class TpgNodeGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgNode',
                         pyname='TpgNode',
                         namespace='DRUID',
                         header_include_files=['types/TpgNode.h'],
                         source_include_files=['pym/PyTpgNode.h',
                                               'pym/PyTpgNodeList.h',
                                               'pym/PyVal3.h',
                                               'pym/PyPrimType.h',
                                               'pym/PyLong.h'])

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

        def get_fanin_num(writer):
            writer.gen_return_py_long('val.fanin_num()')
        self.add_getter('get_fanin_num',
                        func_body=get_fanin_num)
        self.add_attr('fanin_num',
                      getter_name='get_fanin_num')

        def meth_fanin(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.fanin(pos)')
        self.add_method('fanin',
                        func_body=meth_fanin,
                        arg_list=[LongArg(name='pos',
                                          cvarname='pos')],
                        doc_str='return fanin node')

        def meth_fanin_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.fanin_list()')
        self.add_method('fanin_list',
                        func_body=meth_fanin_list,
                        doc_str='return list of fanin nodes')

        def get_fanout_num(writer):
            writer.gen_return_py_long('val.fanout_num()')
        self.add_getter('get_fanout_num',
                        func_body=get_fanout_num)
        self.add_attr('fanout_num',
                      getter_name='get_fanout_num')

        def meth_fanout(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.fanout(pos)')
        self.add_method('fanout',
                        func_body=meth_fanout,
                        arg_list=[LongArg(name='pos',
                                          cvarname='pos')],
                        doc_str='return fanout node')

        def meth_fanout_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.fanout_list()')
        self.add_method('fanout_list',
                        func_body=meth_fanout_list,
                        doc_str='return list of fanout nodes')

        def get_ffr_root(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.ffr_root()')
        self.add_getter('get_ffr_root',
                        func_body=get_ffr_root)
        self.add_attr('ffr_root',
                      getter_name='get_ffr_root')

        def get_mffc_root(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.mffc_root()')
        self.add_getter('get_mffc_root',
                        func_body=get_mffc_root)
        self.add_attr('mffc_root',
                      getter_name='get_mffc_root')

        def get_imm_dom(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.imm_dom()')
        self.add_getter('get_imm_dom',
                        func_body=get_imm_dom)
        self.add_attr('imm_dom',
                      getter_name='get_imm_dom')

        def meth_is_primary_input(writer):
            writer.gen_return_py_bool('val.is_primary_input()')
        self.add_method('is_primary_input',
                        func_body=meth_is_primary_input,
                        doc_str='True if Primary Input')

        def meth_is_primary_output(writer):
            writer.gen_return_py_bool('val.is_primary_output()')
        self.add_method('is_primary_output',
                        func_body=meth_is_primary_output,
                        doc_str='True if Primary Output')

        def meth_is_dff_input(writer):
            writer.gen_return_py_bool('val.is_dff_input()')
        self.add_method('is_dff_input',
                        func_body=meth_is_dff_input,
                        doc_str='True if DFF Input')

        def meth_is_dff_output(writer):
            writer.gen_return_py_bool('val.is_dff_output()')
        self.add_method('is_dff_output',
                        func_body=meth_is_dff_output,
                        doc_str='True if DFF Output')

        def meth_is_ppi(writer):
            writer.gen_return_py_bool('val.is_ppi()')
        self.add_method('is_ppi',
                        func_body=meth_is_ppi,
                        doc_str='True if Pseudo Primary Input')

        def meth_is_ppo(writer):
            writer.gen_return_py_bool('val.is_ppo()')
        self.add_method('is_ppo',
                        func_body=meth_is_ppo,
                        doc_str='True if Pseudo Primary Output')

        def meth_is_logic(writer):
            writer.gen_return_py_bool('val.is_logic()')
        self.add_method('is_logic',
                        func_body=meth_is_logic,
                        doc_str='True if Logic type')

        def get_input_id(writer):
            writer.gen_return_py_long('val.input_id()')
        self.add_getter('get_input_id',
                        func_body=get_input_id)
        self.add_attr('input_id',
                      getter_name='get_input_id')

        def get_output_id(writer):
            writer.gen_return_py_long('val.output_id()')
        self.add_getter('get_output_id',
                        func_body=get_output_id)
        self.add_attr('output_id',
                      getter_name='get_output_id')

        def get_output_id2(writer):
            writer.gen_return_py_long('val.output_id2()')
        self.add_getter('get_output_id2',
                        func_body=get_output_id2)
        self.add_attr('output_id2',
                      getter_name='get_output_id2')

        def get_dff_id(writer):
            writer.gen_return_py_long('val.dff_id()')
        self.add_getter('get_dff_id',
                        func_body=get_dff_id)
        self.add_attr('dff_id',
                      getter_name='get_dff_id')

        def get_alt_node(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.alt_node()')
        self.add_getter('get_alt_node',
                        func_body=get_alt_node)
        self.add_attr('alt_node',
                      getter_name='get_alt_node')

        def get_gate_type(writer):
            writer.gen_return_pyobject('PyPrimType',
                                       'val.gate_type()')
        self.add_getter('get_gate_type',
                        func_body=get_gate_type)
        self.add_attr('gate_type',
                      getter_name='get_gate_type')

        def get_cval(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.cval()')
        self.add_getter('get_cval',
                        func_body=get_cval)
        self.add_attr('cval',
                      getter_name='get_cval')

        def get_nval(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.nval()')
        self.add_getter('get_nval',
                        func_body=get_nval)
        self.add_attr('nval',
                      getter_name='get_nval')

        def get_coval(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.coval()')
        self.add_getter('get_coval',
                        func_body=get_coval)
        self.add_attr('coval',
                      getter_name='get_coval')

        def get_noval(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.noval()')
        self.add_getter('get_noval',
                        func_body=get_noval)
        self.add_attr('noval',
                      getter_name='get_noval')

        self.add_conv('default')
        self.add_deconv('default')
