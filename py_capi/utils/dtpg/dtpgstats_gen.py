#! /usr/bin/env python3

""" PyDtpgStats を生成するスクリプト

:file: dtpgstats_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import DoubleArg, UlongArg
from sat import SatStatsArg
from .dtpg_args import DtpgStatsArg


class DtpgStatsGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='DtpgStats',
                         pyname='DtpgStats',
                         namespace='DRUID',
                         header_include_files=['dtpg/DtpgStats.h'],
                         source_include_files=['pym/PyDtpgStats.h',
                                               'pym/PySatStats.h',
                                               'pym/PyUlong.h',
                                               'pym/PyFloat.h'])

        self.add_dealloc('default')

        def get_total_count(writer):
            writer.gen_return_py_ulong('val.total_count()')
        self.add_getter('get_total_count',
                        func_body=get_total_count)
        self.add_attr('total_count',
                      getter_name='get_total_count',
                      doc_str='the number of total faults')

        def get_detect_count(writer):
            writer.gen_return_py_ulong('val.detect_count()')
        self.add_getter('get_detect_count',
                        func_body=get_detect_count)
        self.add_attr('detect_count',
                      getter_name='get_detect_count',
                      doc_str='the number of detected faults')

        def get_detect_time(writer):
            writer.gen_return_py_float('val.detect_time()')
        self.add_getter('get_detect_time',
                        func_body=get_detect_time)
        self.add_attr('detect_time',
                      getter_name='get_detect_time',
                      doc_str='CPU time for detected faults')

        def get_untest_count(writer):
            writer.gen_return_py_ulong('val.untest_count()')
        self.add_getter('get_untest_count',
                        func_body=get_untest_count)
        self.add_attr('untest_count',
                      getter_name='get_untest_count',
                      doc_str='the number of untestable faults')

        def get_untest_time(writer):
            writer.gen_return_py_float('val.untest_time()')
        self.add_getter('get_untest_time',
                        func_body=get_untest_time)
        self.add_attr('untest_time',
                      getter_name='get_untest_time',
                      doc_str='CPU time for untestable faults')

        def get_abort_count(writer):
            writer.gen_return_py_ulong('val.abort_count()')
        self.add_getter('get_abort_count',
                        func_body=get_abort_count)
        self.add_attr('abort_count',
                      getter_name='get_abort_count',
                      doc_str='the number of aborted faults')

        def get_abort_time(writer):
            writer.gen_return_py_float('val.abort_time()')
        self.add_getter('get_abort_time',
                        func_body=get_abort_time)
        self.add_attr('abort_time',
                      getter_name='get_abort_time',
                      doc_str='CPU time for aborted faults')

        def get_cnfgen_count(writer):
            writer.gen_return_py_ulong('val.cnfgen_count()')
        self.add_getter('get_cnfgen_count',
                        func_body=get_cnfgen_count)
        self.add_attr('cnfgen_count',
                      getter_name='get_cnfgen_count',
                      doc_str='CNF generation count')

        def get_cnfgen_time(writer):
            writer.gen_return_py_float('val.cnfgen_time()')
        self.add_getter('get_cnfgen_time',
                        func_body=get_cnfgen_time)
        self.add_attr('cnfgen_time',
                      getter_name='get_cnfgen_time',
                      doc_str='CPU time for CNF generation')

        def get_sat_stats(writer):
            writer.gen_return_pyobject('PySatStats',
                                       'val.sat_stats()')
        self.add_getter('get_sat_stats',
                        func_body=get_sat_stats)
        self.add_attr('sat_stats',
                      getter_name='get_sat_stats',
                      doc_str='SAT statictics')

        def get_sat_stats_max(writer):
            writer.gen_return_pyobject('PySatStats',
                                       'val.sat_stats_max()')
        self.add_getter('get_sat_stats_max',
                        func_body=get_sat_stats_max)
        self.add_attr('sat_stats_max',
                      getter_name='get_sat_stats_max',
                      doc_str='max SAT statictics')

        def get_backtrace_time(writer):
            writer.gen_return_py_float('val.backtrace_time()')
        self.add_getter('get_backtrace_time',
                        func_body=get_backtrace_time)
        self.add_attr('backtrace_time',
                      getter_name='get_backtrace_time',
                      doc_str='CPU time for backtrace')

        def meth_clear(writer):
            writer.gen_stmt('val.clear()')
            writer.gen_return_py_none()
        self.add_method('clear',
                        func_body=meth_clear,
                        doc_str='clear')

        def meth_update_det(writer):
            writer.gen_stmt('val.update_det(sat_time, backtrace_time)')
            writer.gen_return_py_none()
        self.add_method('update_det',
                        func_body=meth_update_det,
                        arg_list=[DoubleArg(name='sat_time',
                                            cvarname='sat_time'),
                                  DoubleArg(name='backtrace_time',
                                            cvarname='backtrace_time')],
                        doc_str='update statistics for detected faults')

        def meth_update_untest(writer):
            writer.gen_stmt('val.update_untest(time)')
            writer.gen_return_py_none()
        self.add_method('update_untest',
                        func_body=meth_update_untest,
                        arg_list=[DoubleArg(name='time',
                                            cvarname='time')],
                        doc_str='update statistics for untestable faults')

        def meth_update_abort(writer):
            writer.gen_stmt('val.update_abort(time)')
            writer.gen_return_py_none()
        self.add_method('update_abort',
                        func_body=meth_update_abort,
                        arg_list=[DoubleArg(name='time',
                                            cvarname='time')],
                        doc_str='update statistics for aborted faults')

        def meth_update_cnf(writer):
            writer.gen_stmt('val.update_cnf(time)')
            writer.gen_return_py_none()
        self.add_method('update_cnf',
                        func_body=meth_update_cnf,
                        arg_list=[DoubleArg(name='time',
                                            cvarname='time')],
                        doc_str='update statistics for CNF generation')

        def meth_merge(writer):
            writer.gen_stmt('val.merge(src)')
            writer.gen_return_py_none()
        self.add_method('merge',
                        func_body=meth_merge,
                        arg_list=[DtpgStatsArg(name='src',
                                               cvarname='src')],
                        doc_str='merge')

        def meth_update_sat_stats(writer):
            writer.gen_stmt('val.update_sat_stats(src_stats)')
            writer.gen_return_py_none()
        self.add_method('update_sat_stats',
                        func_body=meth_update_sat_stats,
                        arg_list=[SatStatsArg(name='src_stats',
                                              cvarname='src_stats')],
                        doc_str='update SAT statictics')

        self.add_conv('default')
        self.add_deconv('default')
