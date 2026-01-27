#! /usr/bin/env python3

""" PyAssignList を生成するスクリプト

:file: assignlist_gen.py
:author: Yusuke Matsunaga (松永 裕介)n
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class AssignListGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='AssignList',
                         pyname='AssignList',
                         namespace='DRUID',
                         header_include_files=['types/AssignList.h'],
                         source_include_files=['pym/PyAssignList.h',
                                               'pym/PyAssign.h',
                                               'pym/PyUlong.h'])

        def sq_length(writer):
            writer.gen_auto_assign('len_val', 'val.size()')
        def sq_item(writer):
            writer.gen_return_pyobject('PyAssign',
                                       'val.elem(index)')
        self.add_sequence(sq_length=sq_length,
                          sq_item=sq_item)

        self.add_conv('default')

        self.add_deconv('default')
