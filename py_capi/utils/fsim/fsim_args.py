#! /usr/bin/env python3

""" fsim で定義された型の引数定義

:file: fsim_args.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import TypedObjConvArg


class DiffBitsArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='DiffBits',
                         cvardefault='{}',
                         pyclassname='PyDiffBits')

class DiffBitsArrayArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='DiffBitsArray',
                         cvardefault='{}',
                         pyclassname='PyDiffbitsArray')
