#! /usr/bin/env python3

""" PyFaultType を生成するスクリプト

:file: faulttype_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import EnumGen, EnumInfo


class FaultTypeGen(EnumGen):

    def __init__(self):
        super().__init__(classname='FaultType',
                         pyname='FaultType',
                         namespace='DRUID',
                         enum_list=[EnumInfo('FaultType::StuckAt',
                                             'StuckAt', 'StuckAt'),
                                    EnumInfo('FaultType::TransitionDelay',
                                             'TransitionDelay', 'TransitionDelay'),
                                    EnumInfo('FaultType::GateExhaustive',
                                             'GateExhaustive', 'GateExhaustive')],
                         none_value='FaultType::None',
                         header_include_files=['types/FaultType.h'],
                         source_include_files=['pym/PyFaultType.h',
                                               'pym/PyString.h'])
