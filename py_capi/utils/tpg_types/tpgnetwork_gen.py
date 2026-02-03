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
                         doc_str=["基本的には TpgNode のネットワーク(DAG)を表す．",
                                  "",
                                  "ただし，フルスキャンの順序回路を扱うためにフリップフロップの入出力をそれぞれ疑似出力，疑似入力としてあつかう．",
                                  "",
                                  "- 本当の入力と疑似入力をあわせて PPI(Pseudo Primary Input) と呼ぶ．",
                                  "- 本当の出力と疑似出力をあわせて PPO(Pseudo Primary Output) と呼ぶ．",
                                  "",
                                  "フリップフロップのクロック系の回路は無視される．",
                                  "セット/リセット系の回路も無視される．",],
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
                               doc_str=["BnModel から変換する．",
                                        ":param BnModel model: 元となる BnModel"])

        def meth_read_blif(writer):
            writer.gen_return_pyobject('PyTpgNetwork',
                                       'TpgNetwork::read_blif(filename, fault_type)')
        self.add_static_method('read_blif',
                               func_body=meth_read_blif,
                               arg_list=[StringArg(name='filename',
                                                   cvarname='filename'),
                                         FaultTypeArg(name='fault_type',
                                                      cvarname='fault_type')],
                               doc_str=["'blif' ファイルを読み込む．",
                                        ":param str filename: blif ファイル名"])

        def meth_is_valid(writer):
            writer.gen_return_py_bool('val.is_valid()')
        self.add_method('is_valid',
                        func_body=meth_is_valid,
                        doc_str='適正な値を持つ時 True を返す．')

        def meth_node_num(writer):
            writer.gen_return_py_ulong('val.node_num()')
        self.add_method('node_num',
                        func_body=meth_node_num,
                        doc_str='ノード数を返す．')

        def meth_node(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.node(pos)')
        self.add_method('node',
                        func_body=meth_node,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["ノード返す．",
                                 ":param int pos: 位置番号 ( 0 <= pos < node_num() )"])

        def meth_node_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.node_list()')
        self.add_method('node_list',
                        func_body=meth_node_list,
                        doc_str='ノードのリストを返す．')

        def meth_input_num(writer):
            writer.gen_return_py_ulong('val.input_num()')
        self.add_method('input_num',
                        func_body=meth_input_num,
                        doc_str='入力数を返す．')

        def meth_input(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.input(pos)')
        self.add_method('input',
                        func_body=meth_input,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["入力ノードを返す．",
                                 ":param int pos: 入力番号 ( 0 <= pos < input_num() )"])

        def meth_input_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.input_list()')
        self.add_method('input_list',
                        func_body=meth_input_list,
                        doc_str='入力ノードのリストを返す．')

        def meth_output_num(writer):
            writer.gen_return_py_ulong('val.output_num()')
        self.add_method('output_num',
                        func_body=meth_output_num,
                        doc_str='出力数を返す．')

        def meth_output(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.output(pos)')
        self.add_method('output',
                        func_body=meth_output,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["出力ノードを返す．",
                                 ":param int pos: 出力番号 ( 0 <= pos < output_num() )"])

        def meth_output_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.output_list()')
        self.add_method('output_list',
                        func_body=meth_output_list,
                        doc_str='出力ノードのリストを返す．')

        def meth_output2(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.output2(pos)')
        self.add_method('output2',
                        func_body=meth_output2,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["出力ノードを返す．(サイズの昇順)",
                                 ":param int pos: 出力番号 ( 0 <= pos < output_num() )"])

        def meth_ppi_num(writer):
            writer.gen_return_py_ulong('val.ppi_num()')
        self.add_method('ppi_num',
                        func_body=meth_ppi_num,
                        doc_str='PPI数(入力数+DFF数)を返す．')

        def meth_ppi(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.ppi(pos)')
        self.add_method('ppi',
                        func_body=meth_ppi,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["PPIノードを返す．",
                                 ":param int pos: PPI番号 ( 0 <= pos < ppi_num() )"])

        def meth_ppi_name(writer):
            writer.gen_return_py_string('val.ppi_name(pos)')
        self.add_method('ppi_name',
                        func_body=meth_ppi,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["PPI名を返す．",
                                 ":param int pos: PPI番号 ( 0 <= pos < ppi_num() )"])

        def meth_ppi_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.ppi_list()')
        self.add_method('ppi_list',
                        func_body=meth_ppi_list,
                        doc_str='PPIノードのリストを返す．')

        def meth_ppo_num(writer):
            writer.gen_return_py_ulong('val.ppo_num()')
        self.add_method('ppo_num',
                        func_body=meth_ppo_num,
                        doc_str='PPO数(出力数+DFF数)を返す．')

        def meth_ppo(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.ppo(pos)')
        self.add_method('ppo',
                        func_body=meth_ppo,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["PPOノードを返す．",
                                 ":param int pos: PPO番号 ( 0 <= pos < ppo_num() )"])

        def meth_ppo_name(writer):
            writer.gen_return_py_string('val.ppo_name(pos)')
        self.add_method('ppo_name',
                        func_body=meth_ppo,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["PPO名を返す．",
                                 ":param int pos: PPO番号 ( 0 <= pos < ppo_num() )"])

        def meth_ppo_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.ppo_list()')
        self.add_method('ppo_list',
                        func_body=meth_ppo_list,
                        doc_str='PPOノードのリストを返す．')

        def meth_mffc_num(writer):
            writer.gen_return_py_ulong('val.mffc_num()')
        self.add_method('mffc_num',
                        func_body=meth_mffc_num,
                        doc_str='MFFC数を返す．')

        def meth_mffc(writer):
            writer.gen_return_pyobject('PyTpgMFFC',
                                       'val.mffc(pos)')
        self.add_method('mffc',
                        func_body=meth_mffc,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["MFFCを返す．",
                                 ":param int pos: MFFC番号 ( 0 <= pos < mffc_num() )"])

        def meth_mffc_list(writer):
            writer.gen_return_pyobject('PyTpgMFFCList',
                                       'val.mffc_list()')
        self.add_method('mffc_list',
                        func_body=meth_mffc_list,
                        doc_str='MFFCのリストを返す．')

        def meth_ffr_num(writer):
            writer.gen_return_py_ulong('val.ffr_num()')
        self.add_method('ffr_num',
                        func_body=meth_ffr_num,
                        doc_str='FFR数を返す．')

        def meth_ffr(writer):
            writer.gen_return_pyobject('PyTpgFFR',
                                       'val.ffr(pos)')
        self.add_method('ffr',
                        func_body=meth_ffr,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["FFRを返す．",
                                 ":param int pos: FFR番号 ( 0 <= pos < ffr_num() )"])

        def meth_ffr_list(writer):
            writer.gen_return_pyobject('PyTpgFFRList',
                                       'val.ffr_list()')
        self.add_method('ffr_list',
                        func_body=meth_ffr_list,
                        doc_str='FFRのリストを返す．')

        def meth_dff_num(writer):
            writer.gen_return_py_ulong('val.dff_num()')
        self.add_method('dff_num',
                        func_body=meth_dff_num,
                        doc_str='DFF数を返す．')

        def meth_dff_input(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.dff_input(pos)')
        self.add_method('dff_input',
                        func_body=meth_dff_input,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["DFFの入力ノードを返す．",
                                 ":param int pos: DFF番号 ( 0 <= pos < dff_num() )"])

        def meth_dff_output(writer):
            writer.gen_return_pyobject('PyTpgNode',
                                       'val.dff_output(pos)')
        self.add_method('dff_output',
                        func_body=meth_dff_output,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["DFFの出力ノードを返す．",
                                 ":param int pos: DFF番号 ( 0 <= pos < dff_num() )"])

        def meth_gate_num(writer):
            writer.gen_return_py_ulong('val.gate_num()')
        self.add_method('gate_num',
                        func_body=meth_gate_num,
                        doc_str='ゲート数を返す．')

        def meth_gate(writer):
            writer.gen_return_pyobject('PyTpgGate',
                                       'val.gate(pos)')
        self.add_method('gate',
                        func_body=meth_gate,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str=["ゲートを返す．",
                                 ":param int pos: ゲート番号 ( 0 <= pos < gate_num() )"])

        def meth_gate_list(writer):
            writer.gen_return_pyobject('PyTpgGateList',
                                       'val.gate_list()')
        self.add_method('gate_list',
                        func_body=meth_gate_list,
                        doc_str='ゲートのリストを返す．')

        def meth_fault_type(writer):
            writer.gen_return_pyobject('PyFaultType',
                                       'val.fault_type()')
        self.add_method('fault_type',
                        func_body=meth_fault_type,
                        doc_str='故障の種類(FaultType)を返す．')

        def meth_has_prev_state(writer):
            writer.gen_return_py_bool('val.has_prev_state()')
        self.add_method('has_prev_state',
                        func_body=meth_has_prev_state,
                        doc_str='1時刻前の状態を持つ時 True を返す．')

        def meth_rep_fault_list(writer):
            writer.gen_return_pyobject('PyTpgFaultList',
                                       'val.rep_fault_list()')
        self.add_method('rep_fault_list',
                        func_body=meth_rep_fault_list,
                        doc_str='代表故障のリストを返す．')

        def meth_max_fault_id(writer):
            writer.gen_return_py_ulong('val.max_fault_id()')
        self.add_method('max_fault_id',
                        func_body=meth_max_fault_id,
                        doc_str='故障番号の最大値を返す．')

        def meth_fault(writer):
            writer.gen_return_pyobject('PyTpgFault',
                                       'val.fault(pos)')
        self.add_method('fault',
                        func_body=meth_fault,
                        arg_list=[UlongArg(name='fault_id',
                                           cvarname='pos')],
                        doc_str=["故障を返す．",
                                 ":param int fault_id: 故障番号 ( 0 <= fault_id <= max_fault_id() )"])

        def meth_print(writer):
            writer.gen_auto_assign('s', 'opyfstream(fout_obj)')
            writer.gen_stmt('val.print(s)')
            writer.gen_return_py_none()
        self.add_method('print',
                        func_body=meth_print,
                        arg_list=[RawObjArg(name='fout',
                                            cvarname='fout_obj')],
                        doc_str=["内容を出力する．",
                                 ":param file_object fout: 出力先のファイルオブジェクト"])

        self.add_conv('default', move=True)
