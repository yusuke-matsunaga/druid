#! /usr/bin/env python3

""" PySuffCond を生成するスクリプト

:file: suffcond_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from tpg_types import AssignListArg


class SuffCondGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='SuffCond',
                         pyname='StructEngine',
                         namespace='DRUID',
                         header_include_files=['dtpg/SuffCond.h'],
                         source_include_files=['pym/PySuffCond.h',
                                               'pym/PyAssignList.h'])

        self.add_dealloc('default')

        def get_main_cond(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.main_cond()')
        self.add_getter('get_main_cond',
                        func_body=get_main_cond)
        self.add_attr('main_cond',
                      getter_name='get_main_cond',
                      doc_str='main condition')

        def get_aux_cond(writer):
            writer.gen_return_pyobject('PyAssignList',
                                       'val.aux_cond()')
        self.add_getter('get_aux_cond',
                        func_body=get_aux_cond)
        self.add_attr('aux_cond',
                      getter_name='get_aux_cond',
                      doc_str='auxially condition')

        self.add_conv('default')
        self.add_deconv('default')
