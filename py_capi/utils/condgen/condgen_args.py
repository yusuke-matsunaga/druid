#! /usr/bin/env python3

"""

:file: condgen_args.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import TypedObjConvArg, ObjConvArg


class DetCondArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='DetCond',
                         pyclassname='PyDetCond')


class DetCondListArg(ObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='std::vector<DetCond>',
                         cvardefault=None,
                         pyclassname='PyList<DetCond, PyDetCond>')


class CondGenMgrArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='CondGenmgr',
                         pyclassname='PyCondGenMgr')


class CondGenStatsArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='CondGenStats',
                         pyclassname='PyCondGenStats')
