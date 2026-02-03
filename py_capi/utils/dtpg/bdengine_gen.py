#! /usr/bin/env python3

""" PyBdEngine を生成するスクリプト

:file: bdengine_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import OptArg, KwdArg, IntArg
from misc import JsonValueArg
from logic import ExprArg
from sat import SatLiteralArg, SatLiteralListArg
from tpg_types import TpgNetworkRefArg, TpgNodeArg, TpgNodeListArg
from tpg_types import AssignArg, AssignListArg
from .structengine_gen import init_common


class BdEngineGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='BdEngine',
                         pyname='BdEngine',
                         namespace='DRUID',
                         header_include_files=['dtpg/BdEngine.h'],
                         source_include_files=['pym/PyBdEngine.h',
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
            writer.gen_tp_alloc(objclass='BdEngine_Object')
            writer.gen_stmt('new (&my_obj->mVal) BdEngine(network, node, option)')
            writer.gen_return_self()
        self.add_new(func_body=new_func,
                     arg_list=[TpgNetworkRefArg(name='network',
                                                cvarname='network'),
                               TpgNodeArg(name='node',
                                          cvarname='node'),
                               OptArg(),
                               KwdArg(),
                               JsonValueArg(name='option',
                                            cvarname='option')])

        self.add_dealloc('default')

        init_common(self)

        def meth_prop_var(writer):
            writer.gen_return_pyobject('PySatLiteral',
                                       'val.prop_var()')
        self.add_method('prop_var',
                        func_body=meth_prop_var,
                        doc_str='return Propagation Variable')

        def meth_extract_sufficient_condition(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.extract_sufficient_condition()')
        self.add_method('extract_sufficient_condition',
                        func_body=meth_extract_sufficient_condition,
                        doc_str='return sufficient condition')
