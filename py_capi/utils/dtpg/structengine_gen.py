#! /usr/bin/env python3

""" PyStructEngine を生成するスクリプト

:file: structengine_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import OptArg, KwdArg, IntArg
from misc import JsonValueArg
from logic import ExprArg
from sat import SatLiteralArg, SatLiteralListArg
from tpg_types import TpgNetworkRefArg, TpgNodeArg, TpgNodeListArg
from tpg_types import AssignArg, AssignListArg



class StructEngineGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='StructEngine',
                         pyname='StructEngine',
                         namespace='DRUID',
                         header_include_files=['dtpg/StructEngine.h'],
                         source_include_files=['pym/PyStructEngine.h',
                                               'pym/PyTpgNetwork.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PyTpgNodeList.h',
                                               'pym/PyAssign.h',
                                               'pym/PyAssignList.h',
                                               'pym/PySatBool3.h',
                                               'pym/PySatLiteral.h',
                                               'pym/PySatLiteralList.h',
                                               'pym/PySatStats.h',
                                               'pym/PyExpr.h',
                                               'pym/PyList.h',
                                               'pym/PyFloat.h',
                                               'pym/PyInt.h',
                                               'pym/PyJsonValue.h'])

        def new_func(writer):
            writer.gen_tp_alloc(objclass='StructEngine_Object')
            writer.gen_stmt('new (&my_obj->mVal) StructEngine(network, option)')
            writer.gen_return_self()
        self.add_new(func_body=new_func,
                     arg_list=[TpgNetworkRefArg(name='network',
                                                cvarname='network'),
                               OptArg(),
                               KwdArg(),
                               JsonValueArg(name='option',
                                            cvarname='option')])

        self.add_dealloc('default')

        def meth_add_cur_node(writer):
            writer.gen_stmt('val.add_cur_node(node)')
            writer.gen_return_py_none()
        self.add_method('add_cur_node',
                        func_body=meth_add_cur_node,
                        arg_list=[TpgNodeArg(name='node',
                                             cvarname='node')],
                        doc_str='add node in the current time')

        def meth_add_cur_node_list(writer):
            writer.gen_stmt('val.add_cur_node_list(node_list)')
            writer.gen_return_py_none()
        self.add_method('add_cur_node_list',
                        func_body=meth_add_cur_node_list,
                        arg_list=[TpgNodeListArg(name='node_list',
                                                 cvarname='node_list')],
                        doc_str='add nodes in the current time')

        def meth_add_prev_node(writer):
            writer.gen_stmt('val.add_prev_node(node)')
            writer.gen_return_py_none()
        self.add_method('add_prev_node',
                        func_body=meth_add_prev_node,
                        arg_list=[TpgNodeArg(name='node',
                                             cvarname='node')],
                        doc_str='add node in the previous time')

        def meth_add_prev_node_list(writer):
            writer.gen_stmt('val.add_prev_node_list(node_list)')
            writer.gen_return_py_none()
        self.add_method('add_prev_node_list',
                        func_body=meth_add_prev_node_list,
                        arg_list=[TpgNodeListArg(name='node_list',
                                                 cvarname='node_list')],
                        doc_str='add nodes in the previous time')

        def meth_update(writer):
            writer.gen_stmt('val.update()')
            writer.gen_return_py_none()
        self.add_method('update',
                        func_body=meth_update,
                        doc_str='udpate')

        def meth_justify(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.justify(assign_list)')
        self.add_method('justify',
                        func_body=meth_justify,
                        arg_list=[AssignListArg(name='assign_list',
                                                cvarname='assign_list')],
                        doc_str='do Justification')

        def meth_get_pi_assign(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.get_pi_assign()')
        self.add_method('get_pi_assign',
                        func_body=meth_get_pi_assign,
                        doc_str='return current assignments on Primary Inputs')

        def meth_conv_to_literal(writer):
            writer.gen_return_pyobject('PySatLiteral',
                                       'val.conv_to_literal(assign)')
        self.add_method('conv_to_ltieral',
                        func_body=meth_conv_to_literal,
                        arg_list=[AssignArg(name='assign',
                                            cvarname='assign')],
                        doc_str='return SAT literal related to the assignment')

        def meth_conv_to_literal_list(writer):
            writer.gen_return_pyobject('PyList<SatLiteral, PySatLiteral>',
                                       'val.conv_to_literal_list(assign_list)')
        self.add_method('conv_to_literal_list',
                        func_body=meth_conv_to_literal_list,
                        arg_list=[AssignListArg(name='assign_list',
                                                cvarname='assign_list')],
                        doc_str='return SAT literals related to the assignments')

        def meth_expr_to_cnf(writer):
            writer.gen_return_pyobject('PyList<SatLiteral, PySatLiteral>',
                                       'val.expr_to_cnf(expr)')
        self.add_method('expr_to_cnf',
                        func_body=meth_expr_to_cnf,
                        arg_list=[ExprArg(name='expr',
                                          cvarname='expr')],
                        doc_str='make CNF related to the expression')

        def get_cur_node_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.cur_node_list()')
        self.add_getter('get_cur_node_list',
                        func_body=get_cur_node_list)
        self.add_attr('cur_node_list',
                      getter_name='get_cur_node_list',
                      doc_str='node list related to the current time')

        def get_prev_node_list(writer):
            writer.gen_return_pyobject('PyTpgNodeList',
                                       'val.prev_node_list()')
        self.add_getter('get_prev_node_list',
                        func_body=get_prev_node_list)
        self.add_attr('prev_node_list',
                      getter_name='get_prev_node_list',
                      doc_str='node list related to the previous time')

        def meth_solve(writer):
            writer.gen_return_pyobject('PySatBool3',
                                       'val.solve(assumptions)')
        self.add_method('solve',
                        func_body=meth_solve,
                        arg_list=[OptArg(),
                                  KwdArg(),
                                  SatLiteralListArg(name='assumptions',
                                                    cvarname='assumptions')],
                        doc_str='solve the problem')

        def get_stats(writer):
            writer.gen_return_pyobject('PySatStats',
                                       'val.get_stats()')
        self.add_getter('get_stats',
                        func_body=get_stats)
        self.add_attr('stats',
                      getter_name='get_stats',
                      doc_str='SAT statistics')

        def meth_gvar(writer):
            writer.gen_return_pyobject('PySatLiteral',
                                       'val.gvar(node)')
        self.add_method('gvar',
                        func_body=meth_gvar,
                        arg_list=[TpgNodeArg(name='node',
                                             cvarname='node')],
                        doc_str='return G Variable for the node')

        def meth_hvar(writer):
            writer.gen_return_pyobject('PySatLiteral',
                                       'val.hvar(node)')
        self.add_method('hvar',
                        func_body=meth_hvar,
                        arg_list=[TpgNodeArg(name='node',
                                             cvarname='node')],
                        doc_str='return H Variable for the node')

        def meth_val(writer):
            writer.gen_return_py_bool('val.val(node, time)')
        self.add_method('val',
                        func_body=meth_val,
                        arg_list=[TpgNodeArg(name='node',
                                             cvarname='node'),
                                  IntArg(name='time',
                                         cvarname='time')],
                        doc_str='return the value of the node')

        def meth_cnf_time(writer):
            writer.gen_return_py_float('val.cnf_time()')
        self.add_method('cnf_time',
                        func_body=meth_cnf_time,
                        doc_str='return CPU time for CNF gneration')
