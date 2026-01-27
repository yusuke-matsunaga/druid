#! /usr/bin/env python3

""" PyTestVector を生成するスクリプト

:file: testvector_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg
from misc import Mt19937Arg
from .types_args import AssignListArg, Val3Arg
from .types_args import TestVectorArg


class TestVectorGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TestVector',
                         pyname='TestVector',
                         namespace='DRUID',
                         header_include_files=['types/TestVector.h'],
                         source_include_files=['pym/PyTestVector.h',
                                               'pym/PyAssignList.h',
                                               'pym/PyMt19937.h',
                                               'pym/PyVal3.h',
                                               'pym/PyString.h',
                                               'pym/PyUlong.h'])

        def get_vector_size(writer):
            writer.gen_return_py_ulong('val.vector_size()')
        self.add_getter('get_vector_size',
                        func_body=get_vector_size)
        self.add_attr('vector_size',
                      getter_name='get_vector_size',
                      doc_str='vector size(bits)')

        def meth_val(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.val(pos)')
        self.add_method('val',
                        func_body=meth_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return bit value')

        def get_input_num(writer):
            writer.gen_return_py_ulong('val.input_num()')
        self.add_getter('get_input_num',
                        func_body=get_input_num)
        self.add_attr('input_num',
                      getter_name='get_input_num',
                      doc_str='input size(bits)')

        def get_dff_num(writer):
            writer.gen_return_py_ulong('val.dff_num()')
        self.add_getter('get_dff_num',
                        func_body=get_dff_num)
        self.add_attr('dff_num',
                      getter_name='get_dff_num',
                      doc_str='DFF size(bits)')

        def get_ppi_num(writer):
            writer.gen_return_py_ulong('val.ppi_num()')
        self.add_getter('get_ppi_num',
                        func_body=get_ppi_num)
        self.add_attr('ppi_num',
                      getter_name='get_ppi_num',
                      doc_str='PPI size(bits)')

        def meth_has_aux_input(writer):
            writer.gen_return_py_bool('val.has_aux_input()')
        self.add_method('has_aux_input',
                        func_body=meth_has_aux_input,
                        doc_str='True if having aux inputs')

        def meth_ppi_val(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.ppi_val(pos)')
        self.add_method('ppi_val',
                        func_body=meth_ppi_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return bit value of PPI')

        def meth_input_val(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.input_val(pos)')
        self.add_method('input_val',
                        func_body=meth_input_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return bit value of input')

        def meth_dff_val(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.dff_val(pos)')
        self.add_method('dff_val',
                        func_body=meth_dff_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return bit value of DFF')

        def meth_aux_input_val(writer):
            writer.gen_return_pyobject('PyVal3',
                                       'val.aux_input_val(pos)')
        self.add_method('aux_input_val',
                        func_body=meth_aux_input_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return bit value of aux input')

        def meth_x_count(writer):
            writer.gen_return_py_ulong('val.x_count()')
        self.add_method('x_count',
                        func_body=meth_x_count,
                        doc_str='return X count')

        def meth_bin_str(writer):
            writer.gen_return_py_string('val.bin_str()')
        self.add_method('bin_str',
                        func_body=meth_bin_str,
                        doc_str='return BIN string representation')

        def meth_hex_str(writer):
            writer.gen_return_py_string('val.hex_str()')
        self.add_method('hex_str',
                        func_body=meth_hex_str,
                        doc_str='return HEX string representation')

        def meth_init(writer):
            writer.gen_stmt('val.init()')
            writer.gen_return_py_none()
        self.add_method('init',
                        func_body=meth_init,
                        doc_str='initialize')

        def meth_set_from_assign_list(writer):
            writer.gen_stmt('val.set_from_assign_list(assign_list)')
            writer.gen_return_py_none()
        self.add_method('set_from_assign_list',
                        func_body=meth_set_from_assign_list,
                        arg_list=[AssignListArg(name='assign_list',
                                                cvarname='assign_list')],
                        doc_str='set from AssignList')

        def meth_set_ppi_val(writer):
            writer.gen_stmt('val.set_ppi_val(pos, v)')
            writer.gen_return_py_none()
        self.add_method('set_ppi_val',
                        func_body=meth_set_ppi_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos'),
                                  Val3Arg(name='val',
                                          cvarname='v')],
                        doc_str='set bit value of PPI')

        def meth_set_input_val(writer):
            writer.gen_stmt('val.set_input_val(pos, v)')
            writer.gen_return_py_none()
        self.add_method('set_input_val',
                        func_body=meth_set_input_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos'),
                                  Val3Arg(name='val',
                                          cvarname='v')],
                        doc_str='set bit value of input')

        def meth_set_dff_val(writer):
            writer.gen_stmt('val.set_dff_val(pos, v)')
            writer.gen_return_py_none()
        self.add_method('set_dff_val',
                        func_body=meth_set_dff_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos'),
                                  Val3Arg(name='val',
                                          cvarname='v')],
                        doc_str='set bit value of DFF')

        def meth_set_aux_input_val(writer):
            writer.gen_stmt('val.set_aux_input_val(pos, v)')
            writer.gen_return_py_none()
        self.add_method('set_aux_input_val',
                        func_body=meth_set_aux_input_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos'),
                                  Val3Arg(name='val',
                                          cvarname='v')],
                        doc_str='set bit value of aux input')

        def meth_set_from_random(writer):
            writer.gen_stmt('val.set_from_random(randgen)')
            writer.gen_return_py_none()
        self.add_method('set_from_random',
                        func_body=meth_set_from_random,
                        arg_list=[Mt19937Arg(name='randgen',
                                             cvarname='randgen')],
                        doc_str='set value from random generator')

        def meth_fix_x_from_random(writer):
            writer.gen_stmt('val.fix_x_from_random(randgen)')
            writer.gen_return_py_none()
        self.add_method('fix_x_from_random',
                        func_body=meth_fix_x_from_random,
                        arg_list=[Mt19937Arg(name='randgen',
                                             cvarname='randgen')],
                        doc_str='fix X value from random generator')

        def meth_check_compatible(writer):
            writer.gen_return_py_bool('val && right')
        self.add_method('check_compatible',
                        func_body=meth_check_compatible,
                        arg_list=[TestVectorArg(name='right',
                                                 cvarname='right')],
                        doc_str='return True if both vectors are compatible')

        def meth_check_containment(writer):
            writer.gen_return_py_bool('val < right')
        self.add_method('check_containment',
                        func_body=meth_check_containment,
                        arg_list=[TestVectorArg(name='right',
                                                 cvarname='right')],
                        doc_str='return True if self is contained in right')

        def meth_check_containment_or_equal(writer):
            writer.gen_return_py_bool('val <= right')
        self.add_method('check_containment_or_equal',
                        func_body=meth_check_containment,
                        arg_list=[TestVectorArg(name='right',
                                                 cvarname='right')],
                        doc_str='return True if self is contained in right or equal')

        def meth_merge(writer):
            writer.gen_return_pyobject('PyTestVector',
                                       'val & right')
        self.add_method('merge',
                        func_body=meth_merge,
                        arg_list=[TestVectorArg(name='right',
                                                cvarname='right')],
                        doc_str='merge')

        self.add_richcompare('eq_default')

        self.add_conv('default')
        self.add_deconv('default')
