#! /usr/bin/env python3

"""

:file: minpat.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

import time
from druid.dtpg import BdEngine, DtpgMgr
from druid.fsim import Fsim
from druid.tpg_types import FaultStatus
from druid.tpg_types import TestVector
from ymworks.sat import SatBool3
from ymworks.combopt import MinCov
from ymworks.misc import JsonValue


def dtpg(network, fault_list, *, option=None):
    """テストパタン生成を行う．
    :param TpgNetwork network: 対象のネットワーク
    :param list[TpgFault] fault_list: 故障のリスト
    :param bool drop: 故障シミュレーションを用いた故障ドロップを行うフラグ
    """

    start_time = time.time()
    if option is not None and option.has_key("dtpg"):
        dtpg_option = option["dtpg"]
    else:
        dtpg_option = None

    if option is not None and option.has_key('drop_limit'):
        drop_limit = option['drop_limit'].get_int()
    else:
        drop_limit = 1

    if option is not None and option.has_key("fsim"):
        fsim_option = option["fsim"]
    else:
        fsim_option = None

    stats, results = DtpgMgr.run(fault_list, option=dtpg_option)
    end_time = time.time()
    dtpg_time = end_time - start_time

    f_id_map = {}
    n_det = 0
    n_untest = 0
    n_abort = 0
    tv_list = []
    for fault in fault_list:
        result = results.status(fault)
        if result == FaultStatus.Detected:
            f_id_map[fault.id] = n_det
            n_det += 1
            tv = results.testvector(fault)
            tv_list.append(tv)
        elif result == FaultStatus.Untestable:
            n_untest += 1
        else:
            n_abort += 1
    print(f'# of Total Faults:      {n_det + n_untest + n_abort:7}')
    print(f'# of Detected Faults:   {n_det:7}')
    print(f'# of Untestable Faults: {n_untest:7}')
    print(f'# of Aborted Faults:    {n_abort:7}')
    print(f'# of testvectors:       {len(tv_list):7}')
    print(f'DTPG time:              {dtpg_time:10.2f}')

    det_count_dict = {}
    for fault in fault_list:
        det_count_dict[fault.id] = 0

    fsim = Fsim(network, fault_list, option=fsim_option)
    start_time = time.time()
    mincov = MinCov()
    n = len(tv_list)
    for base in range(0, n, 64):
        end = min(base + 64, n)
        tv_buff64 = tv_list[base: end]
        fsim_res = fsim.ppsfp(tv_buff64)
        for tv_id in range(len(tv_buff64)):
            for fid in fsim_res.fault_list(tv_id):
                det_count_dict[fid] += 1
                mincov.insert_elem(row_pos=fid, col_pos=(tv_id + base))
                if drop_limit > 0 and det_count_dict[fid] >= drop_limit:
                    fsim.set_skip(network.fault(fid))
    end_time = time.time()
    fsim_time = end_time - start_time
    print(f'Simulation Time:        {fsim_time:10.2f}')

    # 最小被覆問題を解く．
    start_time = time.time()
    option = JsonValue.object()
    option.add_with_key('algorithm', 'greedy')
    if option.has_key('debug'):
        option.add_with_key('debug', option['debug'].get_int())
    solution, nc = mincov.solve(option=option)
    end_time = time.time()
    print(f'Minimum Coverinng End.')
    print(f'# of Patterns: {nc}')
    mincov_time = end_time - start_time
    print(f' Time:                  {mincov_time:10.2f}')


if __name__ == '__main__':

    import sys
    from argparse import ArgumentParser
    from druid.tpg_types import TpgNetwork, FaultType

    parser = ArgumentParser(
        prog='dtpg_test',
        description='test program for DTPG')

    parser.add_argument('filename')
    parser.add_argument('--blif', action='store_true')
    parser.add_argument('--iscas89', action='store_true')
    parser.add_argument('--stuck-at', action='store_true')
    parser.add_argument('--transition-delay', action='store_true')
    parser.add_argument('--mode', type=str)
    parser.add_argument('--drop-limit', type=int)
    parser.add_argument('--fsim-mt', action='store_true')
    parser.add_argument('--debug', type=int)

    args = parser.parse_args()
    if args is None:
        exit(1)

    # ファイル名
    filename = args.filename

    # ファイル形式
    format = 'blif'
    if args.blif:
        if args.iscas89:
            print("'--blif' and '--iscas89' are mutually exclusive")
            exit(1)
        pass
    elif args.iscas89:
        format = 'iscas89'

    # 故障の種類
    if args.stuck_at:
        if args.transition_delay:
            print("'--stuck-at' and '--transition-delay' are mutually exclusive")
            exit(1)
        fault_type = FaultType.StuckAt
    elif args.transition_delay:
        fault_type = FaultType.TransitionDelay
    else:
        # 両方指定されていない場合は StuckAt をデフォルトとする．
        fault_type = FaultType.StuckAt

    # DTPG モード
    if args.mode:
        mode = args.mode
    else:
        mode = "ffr_mt"

    network = TpgNetwork.read_network(filename, format, fault_type)
    fault_list = network.rep_fault_list()

    option = JsonValue.object()
    if args.drop_limit:
        option.add_with_key('drop_limit', args.drop_limit)
    if args.fsim_mt:
        fsim_option = JsonValue({"multi_thread": JsonValue(True)})
        option.add_with_key("fsim", fsim_option)
    if args.debug:
        option.add_with_key('debug', args.debug)
    dtpg_option = JsonValue.object()
    dtpg_option.add_with_key("group_mode", mode);
    option.add_with_key("dtpg", dtpg_option)
    dtpg(network, fault_list, option=option)
