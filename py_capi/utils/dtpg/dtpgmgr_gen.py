#! /usr/bin/env python3

""" PyDtpgMgr を生成するスクリプト

:file: dtpgmgr_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import OptArg, KwdArg
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
                                               'pym/PyTpgFaultList.h',
                                               'pym/PyDtpgResults.h',
                                               'pym/PyJsonValue.h'])

        def meth_run(writer):
            writer.gen_vardecl(typename='DtpgResults',
                               varname='results')
            writer.gen_return_pyobject('PyDtpgResults',
                                       'DtpgMgr::run(fault_list, option)')
        self.add_static_method('run',
                               func_body=meth_run,
                               arg_list=[TpgFaultListArg(name='fault_list',
                                                         cvarname='fault_list'),
                                         OptArg(),
                                         KwdArg(),
                                         JsonValueArg(name='option',
                                                      cvarname='option')],
                               doc_str='do DTPG')
