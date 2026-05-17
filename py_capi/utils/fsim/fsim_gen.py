#! /usr/bin/env python3

""" PyFsim を生成するスクリプト

:file: fsim_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg, BoolArg, RawObjArg, OptArg, KwdArg
from misc import JsonValueArg
from tpg_types import TpgNetworkRefArg
from tpg_types import TpgFaultListArg, TpgFaultArg
from tpg_types import TestVectorArg, TestVectorListArg, AssignListArg


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
                                               'pym/PyFsimResults.h',
                                               'pym/PyJsonValue.h',
                                               'pym/PyList.h',
                                               'pym/PyBool.h',
                                               'pym/PyUlong.h'])
        def preamble_body(writer):
            writer.gen_include('parse_faults.cc')
        self.add_preamble(preamble_body)

        self.add_dealloc('default')

        def new_body(writer):
            writer.gen_auto_assign('self', 'type->tp_alloc(type, 0)')
            self.gen_obj_conv(writer, objname='self', varname='my_obj')
            writer.gen_stmt('new (&my_obj->mVal) Fsim(fault_list, option)')
            writer.gen_return_self()
        self.add_new(new_body,
                     arg_list=[TpgFaultListArg(name='fault_list',
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
            with writer.gen_if_block('tv.vector_size() > 0'):
                writer.gen_return_py_bool('val.spsfp(tv, fault)')
            with writer.gen_if_block('as_list.size() > 0'):
                writer.gen_return_py_bool('val.spsfp(as_list, fault)')
            writer.gen_type_error('"either testvector or assign_list must be given"')
        self.add_method('spsfp',
                        func_body=meth_spsfp,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault'),
                                  OptArg(),
                                  KwdArg(),
                                  TestVectorArg(name='testvector',
                                                cvarname='tv'),
                                  AssignListArg(name='assign_list',
                                                cvarname='as_list')],
                        doc_str='do SPSFP fault simulation')

        def meth_spsfp2(writer):
            with writer.gen_if_block('tv.vector_size() > 0'):
                writer.gen_return_pyobject('PyDiffBits',
                                           'val.spsfp2(tv, fault)')
            with writer.gen_if_block('as_list.size() > 0'):
                writer.gen_return_pyobject('PyDiffBits',
                                           'val.spsfp2(as_list, fault)')
            writer.gen_type_error('"either testvector or assign_list must be given"')
        self.add_method('spsfp2',
                        func_body=meth_spsfp2,
                        arg_list=[TpgFaultArg(name='fault',
                                              cvarname='fault'),
                                  OptArg(),
                                  KwdArg(),
                                  TestVectorArg(name='testvector',
                                                cvarname='tv'),
                                  AssignListArg(name='assign_list',
                                                cvarname='as_list')],
                        doc_str='do SPSFP fault simulation')

        def meth_run_single(writer):
            with writer.gen_if_block('tv.vector_size() > 0'):
                writer.gen_return_pyobject('PyTpgFaultList',
                                           'val.run_single(tv)')
            with writer.gen_if_block('as_list.size() > 0'):
                writer.gen_return_pyobject('PyTpgFaultList',
                                           'val.run_single(as_list)')
            writer.gen_type_error('"either testvector or assign_list must be given"')
        self.add_method('run_single',
                        func_body=meth_run_single,
                        arg_list=[OptArg(),
                                  KwdArg(),
                                  TestVectorArg(name='testvector',
                                                cvarname='tv'),
                                  AssignListArg(name='assign_list',
                                                cvarname='as_list')],
                        doc_str='do single pattern fault simulation')

        def meth_run_multi(writer):
            writer.gen_return_pyobject('PyList<TpgFaultList, PyTpgFaultList>',
                                       'val.run_multi(tv_list, ppsfp)')
        self.add_method('run_multi',
                        func_body=meth_run_multi,
                        arg_list=[KwdArg(),
                                  TestVectorListArg(name='tv_list',
                                                    cvarname='tv_list'),
                                  OptArg(),
                                  BoolArg(name='ppsfp',
                                          cvarname='ppsfp')],
                        doc_str='do multiple pattern fault simulation')

        def meth_run_single2(writer):
            with writer.gen_if_block('tv.vector_size() > 0'):
                writer.gen_return_pyobject('PyFsimResults',
                                           'val.run_single2(tv)')
            with writer.gen_if_block('as_list.size() > 0'):
                writer.gen_return_pyobject('PyFsimResults',
                                           'val.run_single2(as_list)')
            writer.gen_type_error('"either testvector or assign_list must be given"')
        self.add_method('run_single2',
                        func_body=meth_run_single2,
                        arg_list=[OptArg(),
                                  KwdArg(),
                                  TestVectorArg(name='testvector',
                                                cvarname='tv'),
                                  AssignListArg(name='assign_list',
                                                cvarname='as_list')],
                        doc_str='do single pattern fault simulation')

        def meth_run_multi2(writer):
            writer.gen_return_pyobject('PyFsimResults',
                                       'val.run_multi2(tv_list, ppsfp)')
        self.add_method('run_multi2',
                        func_body=meth_run_multi2,
                        arg_list=[KwdArg(),
                                  TestVectorListArg(name='tv_list',
                                                    cvarname='tv_list'),
                                  OptArg(),
                                  BoolArg(name='ppsfp',
                                          cvarname='ppsfp')],
                        doc_str='do multiple pattern fault simulation')

        def meth_calc_wsa(writer):
            pass

        def meth_set_state(writer):
            pass

        def meth_get_state(writer):
            pass
