#! /usr/bin/env python3

""" PyDiffBitsArray を生成するスクリプト

:file: diffbitsarray_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg, RawObjArg
from .fsim_args import DiffBitsArg

class DiffBitsArrayGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='DiffBitsArray',
                         pyname='DiffBitsArray',
                         namespace='DRUID',
                         header_include_files=['fsim/DiffBitsArray.h'],
                         source_include_files=['pym/PyDiffBitsArray.h',
                                               'pym/PyDiffBits.h',
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

        def meth_dbits(writer):
            writer.gen_return_py_ulong('val.dbits(pos)')
        self.add_method('dbits',
                        func_body=meth_dbits,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return DIFF bits')

        def meth_dbits_union(writer):
            writer.gen_return_py_ulong('val.dbits_union()')
        self.add_method('dbits_union',
                        func_body=meth_dbits_union,
                        doc_str='return union of DIFF bits of all outputs')

        def meth_get_slice(writer):
            writer.gen_return_pyobject('PyDiffBits',
                                       'val.get_slice(pos)')
        self.add_method('get_slice',
                        func_body=meth_get_slice,
                        arg_list=[UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return bit slice')

        def meth_masking(writer):
            writer.gen_return_pyobject('PyDiffBitsArray',
                                       'val.masking(mask)')
        self.add_method('masking',
                        func_body=meth_masking,
                        arg_list=[UlongArg(name='mask',
                                           cvarname='mask')],
                        doc_str='return masked DiffBitsArray')

        def meth_clear(writer):
            writer.gen_stmt('val.clear()')
            writer.gen_return_py_none()
        self.add_method('clear',
                        func_body=meth_clear,
                        doc_str='clear')

        def meth_add_output(writer):
            writer.gen_stmt('val.add_output(output, dbits)')
            writer.gen_return_py_none()
        self.add_method('add_output',
                        func_body=meth_add_output,
                        arg_list=[UlongArg(name='output',
                                           cvarname='output'),
                                  UlongArg(name='dbits',
                                           cvarname='dbits')],
                        doc_str='add output ID')

        def meth_add_pat(writer):
            writer.gen_stmt('val.add_pat(dbits, pos)')
            writer.gen_return_py_none()
        self.add_method('add_pat',
                        func_body=meth_add_pat,
                        arg_list=[DiffBitsArg(name='dbits',
                                              cvarname='dbits'),
                                  UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='add bit slice')

        def meth_sort(writer):
            writer.gen_stmt('val.sort()')
            writer.gen_return_py_none()
        self.add_method('sort',
                        func_body=meth_sort,
                        doc_str='do sorting')

        def meth_sorted(writer):
            writer.gen_return_pyobject('PyDiffBitsArray',
                                       'val.sorted()')
        self.add_method('sorted',
                        func_body=meth_sorted,
                        doc_str='return sorted DiffBitsArray')

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
