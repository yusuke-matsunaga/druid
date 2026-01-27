#! /usr/bin/env python3

"""

:file: types_arg.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import TypedObjConvArg, TypedObjRefArg, ObjConvArg


class Val3Arg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='Val3',
                         cvardefault='Val3::_X',
                         pyclassname='PyVal3')


class Fval2Arg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='Fval2',
                         cvardefault='Fval2::zero',
                         pyclassname='PyFval2')


class FaultStatusArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='FaultStatus',
                         cvardefault='FaultStatus::Undetected',
                         pyclassname='PyFaultStatus')


class FaultTypeArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='FaultType',
                         cvardefault='FaultType::None',
                         pyclassname='PyFaultType')


class TpgNetworkRefArg(TypedObjRefArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         pyclassname='PyTpgNetwork')


class TpgNodeArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='TpgNode',
                         cvardefault=None,
                         pyclassname='PyTpgNode')


class TpgNodeListArg(TypedObjRefArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         pyclassname='PyTpgNodeList')


class TpgFaultArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='TpgFault',
                         cvardefault=None,
                         pyclassname='PyTpgFault')

class TpgFaultListArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='TpgFaultList',
                         cvardefault=None,
                         pyclassname='PyTpgFaultList')

class TestVectorArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='TestVector',
                         cvardefault=None,
                         pyclassname='PyTestVector')

class TestVectorListArg(ObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='std::vector<TestVector>',
                         cvardefault=None,
                         pyclassname='PyList<TestVector, PyTestVector>')

class InputVectorArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='InputVector',
                         cvardefault=None,
                         pyclassname='PyInputVector')

class DffVectorArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='DffVector',
                         cvardefault=None,
                         pyclassname='PyDffVector')

class AssignArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='Assign',
                         cvardefault=None,
                         pyclassname='PyAssign')

class AssignListArg(TypedObjConvArg):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvarname=cvarname,
                         cvartype='AssignList',
                         cvardefault=None,
                         pyclassname='PyAssignList')
