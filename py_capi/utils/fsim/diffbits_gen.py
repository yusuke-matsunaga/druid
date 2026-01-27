#! /usr/bin/env python3

""" PyDiffBits を生成するスクリプト

:file: diffbits_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg, RawObjArg


class DiffBitsGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='DiffBits',
                         pyname='DiffBits',
                         namespace='DRUID',
                         header_include_files=['fsim/DiffBits.h'],
                         source_include_files=['pym/PyDiffBits.h',
                                               'pym/pyfstream.h',
                                               'pym/PyUlong.h'])

        self.add_dealloc('default')

        def get_elem_num(writer):
            writer.gen_return_py_ulong('val.elem_num()')
        self.add_getter('get_elem_num',
                        func_body=get_elem_num)
        self.add_attr('elem_num',
                      getter_name='get_elem_num')

        def meth_output(writer):
            writer.gen_return_py_ulong('val.output(pos)')
        self.add_method('output',
                        func_body=meth_output,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return output ID')

        def meth_clear(writer):
            writer.gen_stmt('val.clear()')
            writer.gen_return_py_none()
        self.add_method('clear',
                        func_body=meth_clear,
                        doc_str='clear')

        def meth_add_output(writer):
            writer.gen_stmt('val.add_output(output)')
            writer.gen_return_py_none()
        self.add_method('add_output',
                        func_body=meth_add_output,
                        arg_list=[UlongArg(name='output',
                                           cvarname='output')],
                        doc_str='add output ID')

        def meth_sort(writer):
            writer.gen_stmt('val.sort()')
            writer.gen_return_py_none()
        self.add_method('sort',
                        func_body=meth_sort,
                        doc_str='do sorting')

        def meth_sorted(writer):
            writer.gen_return_pyobject('PyDiffBits',
                                       'val.sorted()')
        self.add_method('sorted',
                        func_body=meth_sorted,
                        doc_str='return sorted DiffBits')

        def meth_print(writer):
            writer.gen_auto_assign('s', 'opyfstream(fout_obj)')
            writer.gen_stmt('val.print(s)')
            writer.gen_return_py_none()
        self.add_method('print',
                        func_body=meth_print,
                        arg_list=[RawObjArg(name='fout',
                                            cvarname='fout_obj')],
                        doc_str='print')

        def hash_func(writer):
            writer.gen_return('val.hash()')
        self.add_hash(hash_func)

        self.add_richcompare('eq_default')

        self.add_conv('default')

        self.add_deconv('default')
