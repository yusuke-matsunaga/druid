#! /usr/bin/env python3

""" PyFsimResults の生成スクリプト

:file: fsimresults_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import UlongArg
from .fsim_args import DiffBitsArg, FsimResultsArg


class FsimResultsGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='FsimResults',
                         pyname='FsimResults',
                         namespace='DRUID',
                         header_include_files=['fsim/FsimResults.h'],
                         source_include_files=['pym/PyFsimResults.h',
                                               'pym/PyDiffBits.h',
                                               'pym/PyTpgFault.h',
                                               'pym/PyList.h',
                                               'pym/PyUlong.h'])

        self.add_dealloc('default')

        def meth_tv_num(writer):
            writer.gen_return_py_ulong('val.tv_num()')
        self.add_method('tv_num',
                        func_body=meth_tv_num,
                        doc_str='return the number of TestVectors')

        def meth_det_num(writer):
            writer.gen_return_py_ulong('val.det_num(tv_id)')
        self.add_method('det_num',
                        func_body=meth_det_num,
                        arg_list=[UlongArg(name='tv_id',
                                           cvarname='tv_id')],
                        doc_str='return the number of detected faults')

        def meth_fault(writer):
            writer.gen_return_pyobject('PyTpgFault',
                                       'val.fault(tv_id, pos)')
        self.add_method('fault',
                        func_body=meth_fault,
                        arg_list=[UlongArg(name='tv_id',
                                           cvarname='tv_id'),
                                  UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return detected fault')

        def meth_diffbits(writer):
            writer.gen_return_pyobject('PyDiffBits',
                                       'val.diffbits(tv_id, pos)')
        self.add_method('diffbits',
                        func_body=meth_diffbits,
                        arg_list=[UlongArg(name='tv_id',
                                           cvarname='tv_id'),
                                  UlongArg(name='pos',
                                           cvarname='pos')],
                        doc_str='return DiffBits of the simulation result')

        self.add_conv('default')
        self.add_deconv('default')
