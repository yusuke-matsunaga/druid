#! /usr/bin/env python3

"""

:file: dtpg.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2026 Yusuke Matsunaga, All rights reserved.
"""

from druid.dtpg import BdEngine
from ymworks.sat import SatBool3


def dtpg(network, fault_list, *, drop=False, dtpg_option=None):
    """テストパタン生成を行う．
    :param TpgNetwork network: 対象のネットワーク
    :param list[TpgFault] fault_list: 故障のリスト
    :param bool drop: 故障シミュレーションを用いた故障ドロップを行うフラグ
    """

    # FFR番号をキーにしてFFR内の故障のリストを持つ辞書
    fault_list_dict = {}
    # 故障が存在するFFRの根のノードのリスト
    root_list = []
    for fault in fault_list:
        root = fault.ffr_root
        if root.id not in fault_list_dict:
            root_list.append(root)
            fault_list_dict[root.id] = []
        fault_list_dict[root.id].append(fault)

    # FFR ごとに処理を行う．
    for root in root_list:
        fault_list1 = fault_list_dict[root.id]
        engine = BdEngine(network, root, option=dtpg_option)

        for fault in fault_list1:
            print(f'fault = {fault}')
            prop_cond = fault.ffr_propagate_condition
            assumptions = engine.conv_to_literal_list(prop_cond)
            assumptions.append(engine.prop_var())
            res = engine.solve(assumptions=assumptions)
            if res == SatBool3.true:
                print('detected')
                assign_list = engine.extract_sufficient_condition()
                assign_list.merge(prop_cond)
                pi_assign_list = engine.justify(assign_list)
            elif res == SatBool3.false:
                print('untestable')
            elif res == SatBool3.x:
                print('aborted')


if __name__ == '__main__':

    import sys
    from argparse import ArgumentParser
    from druid.tpg_types import TpgNetwork, FaultType

    parser = ArgumentParser(
        prog='dtpg_teset',
        description='test program for DTPG')

    parser.add_argument('filename')
    parser.add_argument('--blif', action='store_true')
    parser.add_argument('--iscas89', action='store_true')
    parser.add_argument('--stuck-at', action='store_true')
    parser.add_argument('--transition-delay', action='store_true')

    args = parser.parse_args()
    if args is None:
        exit(1)

    # ファイル名
    filename = args.filename

    # ファイル形式
    blif = args.blif
    iscas89 = args.iscas89
    if blif and iscas89:
        print("'--blif' and '--iscas89' are mutually exclusive")
        exit(1)
    if not blif and not iscas89:
        # 両方指定されていない場合は blif をデフォルトにする．
        blif = True

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

    network = TpgNetwork.read_blif(filename, fault_type)
    fault_list = network.rep_fault_list()

    dtpg(network, fault_list)
