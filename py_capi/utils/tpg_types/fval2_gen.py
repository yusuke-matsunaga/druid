#! /usr/bin/env python3

""" PyFval2 を生成するスクリプト

:file: fval2_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import EnumGen, EnumInfo


class Fval2Gen(EnumGen):

    def __init__(self):
        super().__init__(classname='Fval2',
                         pyname='Fval2',
                         namespace='DRUID',
                         enum_list=[EnumInfo('Fval2::zero', 'zero', 'zero'),
                                    EnumInfo('Fval2::one', 'one', 'one')],
                         ignore_case=True,
                         header_include_files=['types/Fval2.h'],
                         source_include_files=['pym/PyFval2.h',
                                               'pym/PyString.h'])
