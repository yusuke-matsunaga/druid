#! /usr/bin/env python3

""" PyFsim を生成するスクリプト

:file: fsim_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg, RawObjArg, OptArg, KwdArg
from misc import JsonValueArg
from tpg_types import TpgNetworkRefArg
from tpg_types import TpgFaultListArg, TpgFaultArg
from tpg_types import TestVectorListArg, AssignListArg


class FaultsParser:
    """1つか複数の TpgFault の引数を読み込むパーサークラス
    """

    def __init__(self, *,
                 cvarname):
        self.__cvarname = cvarname

    def has_args(self):
        return True

    def has_keywords(self):
        return False

    def __call__(self, writer):
        writer.gen_vardecl(typename='TpgFaultList',
                           varname=self.__cvarname)
        with writer.gen_if_block(f'!parse_faults(args, {self.__cvarname})'):
            writer.gen_return('nullptr')


class FsimGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='Fsim',
                         pyname='Fsim',
                         namespace='DRUID',
                         header_include_files=['fsim/Fsim.h'],
                         source_include_files=['pym/PyFsim.h',
                                               'pym/PyTpgNetwork.h',
                                               'pym/PyTpgFault.h',
                                               'pym/PyTpgFaultList.h',
                                               'pym/PyTestVector.h',
                                               'pym/PyAssignList.h',
                                               'pym/PyDiffBits.h',
                                               'pym/PyDiffBitsArray.h',
                                               'pym/PyJsonValue.h',
                                               'pym/PyList.h',
                                               'pym/PyBool.h',
                                               'pym/PyUlong.h'])
        def preamble_body(writer):
            writer.gen_include('parse_faults.cc')
            writer.gen_include('cbfunc.cc')
        self.add_preamble(preamble_body)

        self.add_dealloc('default')

        def new_body(writer):
            writer.gen_auto_assign('self', 'type->tp_alloc(type, 0)')
            self.gen_obj_conv(writer, objname='self', varname='my_obj')
            writer.gen_stmt('new (&my_obj->mVal) Fsim(network, fault_list, option)')
            writer.gen_return_self()
        self.add_new(new_body,
                     arg_list=[TpgNetworkRefArg(name='network',
                                                cvarname='network'),
                               TpgFaultListArg(name='fault_list',
                                               cvarname='fault_list'),
                               OptArg(),
                               KwdArg(),
                               JsonValueArg(name='option',
                                            cvarname='option')])

        def meth_set_skip_all(writer):
            writer.gen_stmt('val.set_skip_all()')
            writer.gen_return_py_none()
        self.add_method('set_skip_all',
                        func_body=meth_set_skip_all,
                        doc_str='set \\"skip\\" mark to all faults')

        def meth_set_skip(writer):
            writer.gen_stmt('val.set_skip(fault_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('set_skip',
                                    func_body=meth_set_skip,
                                    arg_parser=FaultsParser(cvarname='fault_list'),
                                    doc_str='set \\"skip\\" mark')

        def meth_clear_skip_all(writer):
            writer.gen_stmt('val.clear_skip_all()')
            writer.gen_return_py_none()
        self.add_method('clear_skip_all',
                        func_body=meth_clear_skip_all,
                        doc_str='clear \\"skip\\" mark to all faults')

        def meth_clear_skip(writer):
            writer.gen_stmt('val.clear_skip(fault_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('clear_skip',
                                    func_body=meth_clear_skip,
                                    arg_parser=FaultsParser(cvarname='fault_list'),
                                    doc_str='clear \\"skip\\" mark')

        def meth_get_skip(writer):
            writer.gen_return_py_bool('val.get_skip(fault)')
        self.add_method('get_skip',
                        func_body=meth_get_skip,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='get \\"skip\\" mark')

        def meth_spsfp(writer):
            def common(writer):
                writer.gen_auto_assign('res_obj', 'PyBool::ToPyObject(res)')
                writer.gen_auto_assign('dbits_obj', 'PyDiffBits::ToPyObject(dbits)')
                writer.gen_return_buildvalue('OO', ['res_obj', 'dbits_obj'])
            with writer.gen_if_block('PyTestVector::Check(tv_obj)'):
                writer.gen_autoref_assign('tv', 'PyTestVector::_get_ref(tv_obj)')
                writer.gen_vardecl(typename='DiffBits', varname='dbits')
                writer.gen_auto_assign('res', 'val.spsfp(tv, fault, dbits)')
                common(writer)
            with writer.gen_if_block('PyAssignList::Check(tv_obj)'):
                writer.gen_autoref_assign('assign_list', 'PyAssignList::_get_ref(tv_obj)')
                writer.gen_vardecl(typename='DiffBits', varname='dbits')
                writer.gen_auto_assign('res', 'val.spsfp(assign_list, fault, dbits)')
                common(writer)
            writer.gen_type_error('"argument 1 should be TestVector or AssignList"')
        self.add_method('spsfp',
                        func_body=meth_spsfp,
                        arg_list=[RawObjArg(name='tv',
                                            cvarname='tv_obj'),
                                  TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='do SPSFP fault simulation')

        def meth_xspsfp(writer):
            writer.gen_vardecl(typename='DiffBits', varname='dbits')
            writer.gen_auto_assign('res', 'val.xspsfp(assign_list, fault, dbits)')
            writer.gen_auto_assign('res_obj', 'PyBool::ToPyObject(res)')
            writer.gen_auto_assign('dbits_obj', 'PyDiffBits::ToPyObject(dbits)')
            writer.gen_return_buildvalue('OO', ['res_obj', 'dbits_obj'])
        self.add_method('xspsfp',
                        func_body=meth_xspsfp,
                        arg_list=[AssignListArg(name='assign_list',
                                                cvarname='assign_list'),
                                  TpgFaultArg(name='fault',
                                              cvarname='fault')],
                        doc_str='do SPSFP fault simulation with X values')

        def meth_sppfp(writer):
            with writer.gen_if_block('!PyCallable_Check(cb_obj)'):
                writer.gen_type_error('"argument 2 should be callable"')
            writer.gen_auto_assign('callback',
                                   'CbFunc1(cb_obj)')
            with writer.gen_if_block('PyTestVector::Check(tv_obj)'):
                writer.gen_autoref_assign('tv',
                                          'PyTestVector::_get_ref(tv_obj)')
                writer.gen_stmt('val.sppfp(tv, callback)')
                writer.gen_return_py_none()
            with writer.gen_if_block('PyAssignList::Check(tv_obj)'):
                writer.gen_autoref_assign('assign_list',
                                          'PyAssignList::_get_ref(tv_obj)')
                writer.gen_stmt('val.sppfp(assign_list, callback)')
                writer.gen_return_py_none()
            writer.gen_type_error('"argument 1 should be TestVector or AssignList"')
        self.add_method('sppfp',
                        func_body=meth_sppfp,
                        arg_list=[RawObjArg(name='tv',
                                            cvarname='tv_obj'),
                                  RawObjArg(name='callback',
                                            cvarname='cb_obj')],
                        doc_str='do SPPFP fault simulation')

        def meth_xsppfp(writer):
            with writer.gen_if_block('!PyCallable_Check(cb_obj)'):
                writer.gen_type_error('"argument 2 should be callable"')
            writer.gen_auto_assign('callback',
                                   'CbFunc1(cb_obj)')
            writer.gen_stmt('val.xsppfp(assign_list, callback)')
            writer.gen_return_py_none()
        self.add_method('xsppfp',
                        func_body=meth_xsppfp,
                        arg_list=[AssignListArg(name='assign_list',
                                                cvarname='assign_list'),
                                  RawObjArg(name='callback',
                                            cvarname='cb_obj')],
                        doc_str='do SPPFP fault simulation with X values')

        def meth_ppsfp(writer):
            with writer.gen_if_block('!PyCallable_Check(cb_obj)'):
                writer.gen_type_error('"argument 2 should be callable"')
            writer.gen_auto_assign('callback',
                                   'CbFunc2(cb_obj)')
            writer.gen_stmt('val.ppsfp(tv_list, callback)')
            writer.gen_return_py_none()
        self.add_method('ppsfp',
                        func_body=meth_ppsfp,
                        arg_list=[TestVectorListArg(name='tv_list',
                                                    cvarname='tv_list'),
                                  RawObjArg(name='callback',
                                            cvarname='cb_obj')],
                        doc_str='do SPPFP fault simulation with X values')

        def meth_calc_wsa(writer):
            pass

        def meth_set_state(writer):
            pass

        def meth_get_state(writer):
            pass
