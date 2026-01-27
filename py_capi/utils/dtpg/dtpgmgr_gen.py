#! /usr/bin/env python3

""" PyDtpgMgr を生成するスクリプト

:file: dtpgmgr_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from my_py_capi import OptArg, KwdArg
from misc import JsonValueArg
from tpg_types import TpgNetworkRefArg
from tpg_types import TpgFaultListArg


class DtpgMgrGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='DtpgMgr',
                         pyname='DtpgMgr',
                         namespace='DRUID',
                         header_include_files=['dtpg/DtpgMgr.h'],
                         source_include_files=['pym/PyDtpgMgr.h',
                                               'pym/PyTpgNetwork.h',
                                               'pym/PyJsonValue.h'])

        def new_func(writer):
            writer.gen_tp_alloc(objclass='DtpgMgr_Object')
            writer.gen_stmt('new (&my_obj->mVal) DtpgMgr(network, fault_list)')
            writer.gen_return_self()
        self.add_new(func_body=new_func,
                     arg_list=[TpgNetworkRefArg(name='network',
                                                cvarname='network'),
                               TpgFaultListArg(name='fault_list',
                                               cvarname='fault_list')])

        self.add_dealloc('default')

        def meth_run(writer):
            if ( untest_obj != nullptr && abort_obj != nullptr ) {
                    writer.gen_return_pyobject('PyDtpgStats',
                                               'val.run([](DtpgMgr& mgr, const TpgFault* f, TestVector rv) { det_func, [](DtpgMgr& mgr, const TpgFault
