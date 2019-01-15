#! /usr/bin/env python3

### @file dtpg.py
### @brief DTPG を行うクラス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018 Yusuke Matsunaga
### All rights reserved.

from druid_core import DtpgFFR, DtpgMFFC
from druid_core import Fsim
from druid_core import FaultStatus
from druid_core import TestVector


### @brief DTPG を行うクラス
class Dtpg :

    ### @brief 初期化
    def __init__(self, network, fault_type) :
        self.__network = network
        self.__fault_type = fault_type
        self.__fsim3 = Fsim('Fsim3', network, fault_type)
        self.__fsim3.clear_skip_all()
        self.__fault_list = []
        self.__tv_list = []
        self.__fault_drop = False
        self.__fault_mark = {}
        for fault in self.__network.rep_fault_list() :
            self.__fault_mark[fault.id] = True

    ### @brief 故障の印を消す．
    def clear_fault_mark(self) :
        for fault in self.__network.rep_fault_list() :
            self.__fault_mark[fault.id] = False
            self.__fsim3.set_skip(fault)

    ### @brief 故障に印をつける．
    def set_fault_mark(self, fault, val) :
        self.__fault_mark[fault.id] = val
        if val :
            self.__fsim3.clear_skip(fault)
        else :
            self.__fsim3.set_skip(fault)

    ### @brief FFR mode でパタン生成を行う．
    def ffr_mode(self, drop) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        self.__fault_drop = drop
        self.__fault_list = []
        self.__tv_list = []
        for ffr in self.__network.ffr_list() :
            dtpg = DtpgFFR(self.__network, self.__fault_type, ffr)
            for fault in ffr.fault_list() :
                if self.__fault_mark[fault.id] :
                    self.__call_dtpg(dtpg, fault)
        return self.__ndet, self.__nunt, self.__nabt

    ### @brief FFR mode でパタン生成を行う．
    def k_ffr_mode(self, k) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        self.__fault_list = []
        self.__tv_list = []
        for ffr in self.__network.ffr_list() :
            dtpg = DtpgFFR(self.__network, self.__fault_type, ffr)
            for fault in ffr.fault_list() :
                if self.__fault_mark[fault.id] :
                    self.__call_dtpg_k(dtpg, fault, k)
        return self.__ndet, self.__nunt, self.__nabt

    ### @brief MFFC mode でパタン生成を行う．
    def mffc_mode(self, drop) :
        self.__ndet = 0
        self.__nunt = 0
        self.__nabt = 0
        self.__fault_drop = drop
        for mffc in self.__network.mffc_list() :
            dtpg = DtpgMFFC(self.__network, self.__fault_type, mffc)
            for fault in mffc.fault_list() :
                if self.__fault_mark[faul.id] :
                    self.__call_dtpg(dtpg, fault)
        return self.__ndet, self.__nunt, self.__nabt

    ### @brief 全モードで共通な処理
    def __call_dtpg(self, dtpg, fault) :
        stat, testvect = dtpg(fault)
        if stat == FaultStatus.Detected :
            self.__ndet += 1
            # fault を検出可能故障と記録
            self.__fsim3.set_skip(fault)
            self.__fault_list.append(fault)
            self.__fault_mark[fault.id] = False
            # fault のパタンとして testvect を記録
            self.__tv_list.append(testvect)
            if self.__fault_drop :
                # このパタンで検出される他の故障を調べる．
                for fault in self.__fsim3.sppfp(testvect) :
                    self.__fsim3.set_skip(fault)
                    self.__fault_list.append(fault)
                    self.__fault_mark[fault.id] = False
                    self.__ndet += 1
        elif stat == FaultStatus.Untestable :
            self.__nunt += 1
            # fault をテスト不能故障と記録
            self.__fsim3.set_skip(fault)
            self.__fault_mark[fault.id] = False
        elif stat == FaultStatus.Undetected :
            self.__nabt += 1
        else :
            assert False

    ### @brief 全モードで共通な処理
    def __call_dtpg_k(self, dtpg, fault, k) :
        stat, testvect_list = dtpg(fault, k)
        if stat == FaultStatus.Detected :
            self.__ndet += 1
            # fault を検出可能故障と記録
            self.__fsim3.set_skip(fault)
            self.__fault_list.append(fault)
            self.__fault_mark[fault.id] = False
            # fault のパタンとして testvect を記録
            for testvect in testvect_list :
                self.__tv_list.append(testvect)
        elif stat == FaultStatus.Untestable :
            self.__nunt += 1
            # fault をテスト不能故障と記録
            self.__fsim3.set_skip(fault)
            self.__fault_mark[fault.id] = False
        elif stat == FaultStatus.Undetected :
            self.__nabt += 1
        else :
            assert False

    ### @brief 検出された故障のリストを返す．
    @property
    def fault_list(self) :
        return self.__fault_list

    ### @brief テストパタンのリストを返す．
    @property
    def tv_list(self) :
        return self.__tv_list
