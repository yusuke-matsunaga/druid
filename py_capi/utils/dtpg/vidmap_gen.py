#! /usr/bin/env python3

""" PyVidMap を生成するスクリプト

:file: vidmap_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from tpg_types import TpgNodeArg


class VidMapGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='VidMap',
                         pyname='VidMap',
                         namespace='DRUID',
                         header_include_files=['dtpg/VidMap.h'],
                         source_include_files=['pym/PyVidMap.h',
                                               'pym/PyTpgNode.h',
                                               'pym/PySatLiteral.h',
                                               'pym/PyUlong.h'])

        self.add_dealloc('default')

        def meth_get(writer):
            writer.gen_return_pyobject('PySatLiteral',
                                       'val(node)')
        self.add_method('get',
                        func_body=meth_get,
                        arg_list=[TpgNodeArg(name='node',
                                             cvarname='node')],
                        doc_str='return SAT literal related to the node')

        self.add_deconv('default')
