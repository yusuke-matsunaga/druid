#! /usr/bin/env python3

""" PyTpgFaultList を生成するスクリプト

:file: tpgfaultlist_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class TpgFaultListGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='TpgFaultList',
                         pyname='TpgFaultList',
                         namespace='DRUID',
                         header_include_files=['types/TpgFaultList.h'],
                         source_include_files=['pym/PyTpgFaultList.h',
                                               'pym/PyTpgFaultIter2.h',
                                               'pym/PyTpgFault.h',
                                               'pym/PyList.h',
                                               'pym/PyUlong.h'])

        self.add_dealloc('default')

        def meth_is_valid(writer):
            writer.gen_return_py_bool('val.is_valid()')
        self.add_method('is_valid',
                        func_body=meth_is_valid,
                        doc_str='True if valid')

        def sq_length(writer):
            writer.gen_auto_assign('len_val', 'val.size()')
        def sq_item(writer):
            writer.gen_return_pyobject('PyTpgFault',
                                       'val[index]')
        self.add_sequence(sq_length=sq_length,
                          sq_item=sq_item)

        def iter_func(writer):
            writer.gen_return_pyobject('PyTpgFaultIter2',
                                       'val.iter()')
        self.add_iter(iter_func)

        self.add_conv('default')

        def deconv_body(writer):
            self.gen_raw_conv(writer)
            with writer.gen_block():
                writer.gen_vardecl(typename='std::vector<TpgFault>',
                                   varname='fault_list')
                with writer.gen_if_block('PyList<TpgFault, PyTpgFault>::FromPyObject(obj, fault_list)'):
                    writer.gen_assign('val', 'TpgFaultList(fault_list)')
                    writer.gen_return('true')
            writer.gen_return('false')
        self.add_deconv(deconv_body)
