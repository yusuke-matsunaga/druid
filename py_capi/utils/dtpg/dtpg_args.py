#! /usr/bin/env python3

"""

:file: types_arg.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import TypedObjConvArg


class StructEngineArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='StructEngine',
                         pyclassname='PyStructEngine')


class DtpgResultArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='DtpgResult',
                         pyclassname='PyDtpgResult')


class DtpgStatsArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvardefault=None,
                         cvartype='DtpgStats',
                         pyclassname='PyDtpgStats')
