#! /usr/bin/env python3
#
# @file exec_one.py
# @brief 1つの回路に対する処理を行うプログラム
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.

import os.path
import time
from druid import Dtpg
from druid import minpat


def exec_one(dtpg, network, fault_type, fault_list, k, algorithm_list, result_dic, verbose) :
    dtpg.clear_fault_mark()
    for fault in fault_list :
        dtpg.set_fault_mark(fault, True)

    if k == 1 :
        dtpg.ffr_mode(False)
    else :
        dtpg.k_ffr_mode(k)

    if verbose :
        print('--[K = {}]----------------------------'.format(k))
        print('# of initial patterns:   {:8d}'.format(len(dtpg.tv_list)))

    for algorithm in algorithm_list :
        if verbose :
            print('-------------------------------------')
            print('Compaction Algorithm:    {}'.format(algorithm))

        start = time.process_time()
        new_tv_list = minpat(dtpg.tv_list, dtpg.fault_list, network, fault_type, algorithm)
        end = time.process_time()
        cpu_time1 = end - start
        nv = len(new_tv_list)
        # 結果を記録
        sig = '{:02}-{}'.format(k, algorithm)
        result_dic[sig] = ( nv, cpu_time1 )

        if  verbose :
            print('# of minimized patterns: {:8d}'.format(nv))
            print('CPU time(compaction):    {:8.2f}'.format(cpu_time1))
            print()
    if verbose :
        print()
