#! /usr/bin/env python3

""" PyDtpgResults を生成するスクリプト

:file: dtpgresults_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import OptArg, KwdArg, TypedRawObjArg
from tpg_types import TpgFaultArg
from tpg_types import TestVectorArg, AssignListArg, FaultStatusArg


class DtpgResultsGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='DtpgResults',
                         pyname='DtpgResults',
                         namespace='DRUID',
                         header_include_files=['dtpg/DtpgResults.h'],
                         source_include_files=['pym/PyDtpgResults.h',
                                               'pym/PyTpgFault.h',
                                               'pym/PyTestVector.h',
                                               'pym/PyAssignList.h',
                                               'pym/PyFaultStatus.h'])

        self.add_dealloc('default')

        def meth_clear(writer):
            writer.gen_stmt('val.clear()')
            writer.gen_return_py_none()
        self.add_method('clear',
                        func_body=meth_clear,
                        doc_str='clear')

        def meth_set_detected(writer):
            writer.gen_stmt('val.set_detected(fault, testvect)')
            writer.gen_return_py_none();
        self.add_method('set_detected',
                        func_body=meth_set_detected,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault'),
                                  TestVectorArg(name='testvect',
                                                cvarname='testvect')],
                        doc_str='set DETECTED')

        def meth_set_untestable(writer):
            writer.gen_stmt('val.set_untestable(fault)')
            writer.gen_return_py_none()
        self.add_method('set_untestable',
                        func_body=meth_set_untestable,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='set UNTESTABLE')

        def meth_status(writer):
            writer.gen_return_pyobject('PyFaultStatus',
                                       'val.status(fault)')
        self.add_method('status',
                        func_body=meth_status,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='get status')

        def meth_has_testvector(writer):
            writer.gen_return_py_bool('val.has_testvector(fault)')
        self.add_method('has_testvector',
                        func_body=meth_has_testvector,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='return True if having a TestVector')

        def meth_testvector(writer):
            writer.gen_return_pyobject('PyTestVector',
                                       'val.testvector(fault)')
        self.add_method('testvector',
                        func_body=meth_testvector,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='return TestVector of the fault')

        def meth_has_assign_list(writer):
            writer.gen_return_py_bool('val.has_assign_list(fault)')
        self.add_method('has_assign_list',
                        func_body=meth_has_assign_list,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='return True if having a AssignList')

        def meth_assign_list(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.assign_list(fault)')
        self.add_method('assign_list',
                        func_body=meth_assign_list,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='return AssignList of the fault')

        self.add_conv('default')
