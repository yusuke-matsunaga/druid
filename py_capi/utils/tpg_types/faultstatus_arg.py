#! /usr/bin/env python3

""" PyFaultStatus を生成するスクリプト

:file: faultstatus_arg.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import EnumGen, EnumInfo


class FaultStatusGen(EnumGen):

    def __init__(self):
        super().__init__(classname='FaultStatus',
                         pyname='FaultStatus',
                         namespace='DRUID',
                         enum_list=[EnumInfo('FaultStatus::Undetected',
                                             'Undetected', 'Undetected'),
                                    EnumInfo('FaultStatus::Detected',
                                             'Detected', 'Detected'),
                                    EnumInfo('FaultStatus::Untestable',
                                             'Untestable', 'Untestable')],
                         header_include_files=['types/FaultStatus.h'],
                         source_include_files=['pym/PyFaultStatus.h',
                                               'pym/PyString.h'])
