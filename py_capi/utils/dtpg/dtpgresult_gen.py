#! /usr/bin/env python3

""" PyDtpgResult を生成するスクリプト

:file: dtpgresult_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import OptArg, KwdArg, TypedRawObjArg
from tpg_types import TestVectorArg, FaultStatusArg


class DtpgResultGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='DtpgResult',
                         pyname='DtpgResult',
                         namespace='DRUID',
                         header_include_files=['dtpg/DtpgResult.h'],
                         source_include_files=['pym/PyDtpgResult.h',
                                               'pym/PyTestVector.h',
                                               'pym/PyFaultStatus.h'])

        self.add_dealloc('default')

        def meth_detected(writer):
            writer.gen_return_pyobject('PyDtpgResult',
                                       'DtpgResult::detected(testvect)')
        self.add_static_method('detected',
                               func_body=meth_detected,
                               arg_list=[TestVectorArg(name='testvect',
                                                       cvarname='testvect')],
                               doc_str='return DETECTED object')

        def meth_untestable(writer):
            writer.gen_return_pyobject('PyDtpgResult',
                                       'DtpgResult::untestable()')
        self.add_static_method('untestable',
                               func_body=meth_untestable,
                               doc_str='return UNTESTABLE object')

        def meth_undetected(writer):
            writer.gen_return_pyobject('PyDtpgResult',
                                       'DtpgResult::undetected()')
        self.add_static_method('undetected',
                               func_body=meth_undetected,
                               doc_str='return UNDETECTED object')

        def meth_set(writer):
            with writer.gen_if_block('status_obj != nullptr'):
                with writer.gen_if_block('testvect_obj != nullptr'):
                    writer.gen_value_error('"status and testvector are mutially exclusive"')
                writer.gen_autoref_assign('status',
                                          'PyFaultStatus::_get_ref(status_obj)')
                writer.gen_stmt('val.set(status)')
            with writer.gen_elseif_block('testvect_obj != nullptr'):
                writer.gen_autoref_assign('testvect',
                                          'PyTestVector::_get_ref(testvect_obj)')
                writer.gen_stmt('val.set(testvect)')
            with writer.gen_else_block():
                writer.gen_value_error('"either status or testvect must be specified"')
            writer.gen_return_py_none()
        self.add_method('set',
                        func_body=meth_set,
                        arg_list=[OptArg(),
                                  KwdArg(),
                                  TypedRawObjArg(name='status',
                                                 cvarname='status_obj',
                                                 pytypename='PyFaultStatus::_typeobject()'),
                                  TypedRawObjArg(name='testvect',
                                                 cvarname='testvect_obj',
                                                 pytypename='PyTestVector::_typeobject()')],
                        doc_str='set status')

        def get_status(writer):
            writer.gen_return_pyobject('PyFaultStatus',
                                       'val.status()')
        self.add_getter('get_status',
                        func_body=get_status)
        self.add_attr('status',
                      getter_name='get_status',
                      doc_str='fault status')

        def get_testvector(writer):
            writer.gen_return_pyobject('PyTestVector',
                                       'val.testvector()')
        self.add_getter('get_testvector',
                        func_body=get_testvector)
        self.add_attr('testvector',
                      getter_name='get_testvector',
                      doc_str='testvector')

        self.add_conv('default')
