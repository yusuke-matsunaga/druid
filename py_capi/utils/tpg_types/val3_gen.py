#! /usr/bin/env python3

""" PyVal3 を生成するスクリプト

:file: val3_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import EnumGen, EnumInfo


class Val3Gen(EnumGen):

    def __init__(self):
        super().__init__(classname='Val3',
                         pyname='Val3',
                         namespace='DRUID',
                         enum_list=[EnumInfo('Val3::_X', '_X', 'X'),
                                    EnumInfo('Val3::_0', '_0', '0'),
                                    EnumInfo('Val3::_1', '_1', '1')],
                         ignore_case=True,
                         header_include_files=['types/Val3.h'],
                         source_include_files=['pym/PyVal3.h',
                                               'pym/PyString.h'])
