#! /usr/bin/env python3

### @file rtpg.py
### @brief RTPG(random test pattern generation)のスクリプト
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

import druid_core

class Rtpg :

    ### @brief 初期化
    def __init__(self, fmgr, tvmgr, fsim) :
        self._FaultMgr = fmgr
        self._TvMgr = tvmgr
        self._Fsim = fsim
        self._RandGen = RandGen()
        self._min_f = 1
        self._max_undet = 4
        self._max_pat = 1024

    ### @brief パタン生成を行う．
    def run(self) :
        fmgr = self._FaultMgr
        tvmgr = self._TvMgr
        fsim = self._Fsim
        randgen = self._RandGen
        fnum = 0

        # fsim の故障フラグを初期化する．
        fsim.set_skip_all()
        for f in fmgr.fault_list() :
            if fmgr.status(f) == kFsUndetected :
                fsim.clear_skip(f)
                fnum += 1

        # TestVector を用意する．
        tv_array = [tvmgr.new_sa_vector() for i in range(0, kPvBitLen)]

        fsim.clear_patterns()
        pat_num = 0
        wpos = 0
        while True :
            if pat_num < max_pat :
                tv = tv_array[wpos]
                tv.set_from_random(randgen)
                pat_num += 1
                wpos += 1
                if wpos < kPvBitLen :
                    continue
            elif wpos == 0 :
                break

            det_count = fsim.sa_ppsfp()
            def_flags = [False for i in range(0, kPvBitLen)]

            num = wpos
            for i in range(0, det_count) :
                f = fsim.det_fault(i)
                fmgr.set_status(f, kFsDetected)
                fsim.set_skip(f)
                dpat = fsim.det_fault_pat(i)

                # dpat の最初の1のビットを調べる．
                first = 0
                while first < num :
                    if dpat & (1 << first) :
                        break
                    first += 1
                else :
                    assert False
                det_flags[first] = True

            for i in range(0, num) :
                if det_flags[i] :
                    # 検出できたパタンは tvlist に移す．
                    tv = tv_array[i]
                    tvlist.append(tv)
                    # tv_array には新しいパタンを補充しておく
                    tv_array[i] = tvmgr.new_sa_vector()
                    epat_num += 1

            fsim.clear_patterns()
            wpos = 0

            total_det_count += det_count
            if det_count > 0 :
                undet_count = 0
            else :
                undet_count += 1

            if total_det_count == fnum :
                # すべての故障を検出した．
                break

            # ループを打ち切る条件をチェックする．
            if self.break_condition(det_count, undet_count) :
                break

        # パタンを削除する．
        for tv in tv_array :
            tvmgr.delete_vector(tv)

    ## @brief ループの打ち切り条件
    def break_condition(self, det_count, undet_count) :
        if det_count < self._min_f :
            return True
        if undet_count > self._max_i :
            return True
