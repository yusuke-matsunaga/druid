#! /usr/bin/env python3

""" PyInputVector を生成するスクリプト

:file: testvector_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg, StringArg
from misc import Mt19937Arg
from .types_args import AssignListArg, Val3Arg
from .types_args import InputVectorArg


class InputVectorGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='InputVector',
                         pyname='InputVector',
                         namespace='DRUID',
                         header_include_files=['types/InputVector.h'],
                         source_include_files=['pym/PyInputVector.h',
                                               'pym/PyAssignList.h',
                                               'pym/PyVal3.h',
                                               'pym/PyMt19937.h',
                                               'pym/PyString.h',
                                               'pym/PyUlong.h'])

        def get_vector_size(writer):
            writer.gen_return_py_ulong('val.len()')
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

        def meth_lshift(writer):
            writer.gen_stmt('val.lshift(new_val)')
            writer.gen_return_py_none()
        self.add_method('lshift',
                        func_body=meth_lshift,
                        arg_list=[Val3Arg(name='new_val',
                                          cvarname='new_val')],
                        doc_str='do left-shift')

        def meth_rshift(writer):
            writer.gen_stmt('val.rshift(new_val)')
            writer.gen_return_py_none()
        self.add_method('rshift',
                        func_body=meth_rshift,
                        arg_list=[Val3Arg(name='new_val',
                                          cvarname='new_val')],
                        doc_str='do right-shift')

        def meth_set_val(writer):
            writer.gen_stmt('val.set_val(pos, v)')
            writer.gen_return_py_none()
        self.add_method('set_val',
                        func_body=meth_set_val,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos'),
                                  Val3Arg(name='val',
                                          cvarname='v')],
                        doc_str='set bit value')

        def meth_set_from_bin(writer):
            writer.gen_stmt('val.set_from_bin(bin_str)')
            writer.gen_return_py_none()
        self.add_method('set_from_bin',
                        func_body=meth_set_from_bin,
                        arg_list=[StringArg(name='bin_str',
                                            cvarname='bin_str')],
                        doc_str='set from BIN string')

        def meth_set_from_hex(writer):
            writer.gen_stmt('val.set_from_hex(hex_str)')
            writer.gen_return_py_none()
        self.add_method('set_from_hex',
                        func_body=meth_set_from_hex,
                        arg_list=[StringArg(name='hex_str',
                                            cvarname='hex_str')],
                        doc_str='set from HEX string')

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
                        arg_list=[InputVectorArg(name='right',
                                                 cvarname='right')],
                        doc_str='return True if both vectors are compatible')

        def meth_check_containment(writer):
            writer.gen_return_py_bool('val < right')
        self.add_method('check_containment',
                        func_body=meth_check_containment,
                        arg_list=[InputVectorArg(name='right',
                                                 cvarname='right')],
                        doc_str='return True if self is contained in right')

        def meth_check_containment_or_equal(writer):
            writer.gen_return_py_bool('val <= right')
        self.add_method('check_containment_or_equal',
                        func_body=meth_check_containment,
                        arg_list=[InputVectorArg(name='right',
                                                 cvarname='right')],
                        doc_str='return True if self is contained in right or equal')

        def meth_merge(writer):
            writer.gen_return_pyobject('PyInputVector',
                                       'val & right')
        self.add_method('merge',
                        func_body=meth_merge,
                        arg_list=[InputVectorArg(name='right',
                                                 cvarname='right')],
                        doc_str='merge')

        self.add_richcompare('eq_default')

        def hash_func(writer):
            writer.gen_return('val.hash()')
        self.add_hash(hash_func)

        self.add_conv('default')
        self.add_deconv('default')
