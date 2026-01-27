#! /usr/bin/env python3

""" PyCondGenStats を生成するスクリプト

:file: condgenstats_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class CondGenStatsGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='CondGenStats',
                         pyname='CondGenStats',
                         namespace='DRUID',
                         header_include_files=['condgen/CondGenStats.h'],
                         source_include_files=['pym/PyCondGenStats.h',
                                               'pym/PyCnfSize.h'])

        self.add_dealloc('default')

        def get_base_size(writer):
            writer.gen_return_pyobject('PyCnfSize',
                                       'val.base_size()')
        self.add_getter('get_base_size',
                        func_body=get_base_size)
        self.add_attr('base_size',
                      getter_name='get_base_size')

        def get_bd_size(writer):
            writer.gen_return_pyobject('PyCnfSize',
                                       'val.bd_size()')
        self.add_getter('get_bd_size',
                        func_body=get_bd_size)
        self.add_attr('bd_size',
                      getter_name='get_bd_size')

        def get_cond_size(writer):
            writer.gen_return_pyobject('PyCnfSize',
                                       'val.cond_size()')
        self.add_getter('get_cond_size',
                        func_body=get_cond_size)
        self.add_attr('cond_size',
                      getter_name='get_cond_size')

        self.add_conv('default')
        self.add_deconv('default')
