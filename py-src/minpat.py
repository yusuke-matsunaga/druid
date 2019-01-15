#! /usr/bin/env python3
#
# @file minpat.py
# @brief パタン数の最小化を行うプログラム
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2018 Yusuke Matsunaga
# All rights reserved.

from compaction import mincov, coloring
from druid_core import MinPatMgr

def minpat(tv_list, fault_list, network, fault_type, cmp_algorithm) :
    if cmp_algorithm == 'mincov+dsatur' :
        tv_list1 = mincov(fault_list, tv_list, network, fault_type)
        tv_list2 = coloring(tv_list1, 'dsatur')
        return tv_list2
    elif cmp_algorithm == 'mincov+isx' :
        tv_list1 = mincov(fault_list, tv_list, network, fault_type)
        tv_list2 = coloring(tv_list1, 'isx')
        return tv_list2
    elif cmp_algorithm == 'dsatur+mincov' :
        tv_list1 = coloring(tv_list, 'dsatur')
        tv_list2 = mincov(fault_list, tv_list1, network, fault_type)
        return tv_list2
    elif cmp_algorithm == 'isx+mincov' :
        tv_list1 = coloring(tv_list, 'isx')
        tv_list2 = mincov(fault_list, tv_list1, network, fault_type)
        return tv_list2
    elif cmp_algorithm == 'dsatur' :
        tv_list1 = coloring(tv_list, 'dsatur')
        return tv_list1
    elif cmp_algorithm == 'isx' :
        tv_list1 = coloring(tv_list, 'isx')
        return tv_list1
    elif cmp_algorithm == 'coloring2' :
        tv_list1 = MinPatMgr.coloring(fault_list, tv_list, network, fault_type)
        return tv_list1
    elif cmp_algorithm == 'mincov' :
        tv_list1 = mincov(fault_list, tv_list, network, fault_type)
        return tv_list1
    elif cmp_algorithm != '' :
        print('Error: unknown algorithm "{}"'.format(cmp_algorithm))
        return None
