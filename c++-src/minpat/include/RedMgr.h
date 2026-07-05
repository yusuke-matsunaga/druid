#ifndef REDMGR_H
#define REDMGR_H

/// @file RedMgr.h
/// @brief RedMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFault.h"
#include "fsim/Fsim.h"
#include "FaultInfo.h"
#include "DPat.h"
#include "ym/Timer.h"


BEGIN_NAMESPACE_DRUID


//////////////////////////////////////////////////////////////////////
/// @class RedMgr RedMgr.h "RedMgr.h"
/// @brief EqGroupMgr, DomMgr の基底クラス
//////////////////////////////////////////////////////////////////////
class RedMgr
{
public:

  /// @brief コンストラクタ
  RedMgr(
    FaultInfo& fault_info, ///< [in] 故障の情報
    Fsim& fsim             ///< [in] 故障シミュレータ
  ) : mFaultInfo{fault_info},
      mFsim{fsim}
  {
  }

  /// @brief デストラクタ
  ~RedMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FaultIfno を返す．
  FaultInfo&
  fault_info()
  {
    return mFaultInfo;
  }

  /// @brief FaultIfno を返す．
  const FaultInfo&
  fault_info() const
  {
    return mFaultInfo;
  }

  /// @brief 対象のネットワークを返す．
  TpgNetwork
  network() const
  {
    return mFaultInfo.network();
  }

  /// @brief 故障番号の最大値を返す．
  SizeType
  max_fault_size() const
  {
    return network().max_fault_id();
  }

  /// @brief 対象の故障が代表故障の時 true を返す．
  bool
  is_rep(
    const TpgFault& fault ///< [in] 対象の故障
  ) const
  {
    return mFaultInfo.is_rep(fault);
  }

  /// @brief 対象の故障に対する代表故障を設定する．
  void
  set_rep(
    const TpgFault& fault,    ///< [in] 対象の故障
    const TpgFault& rep_fault ///< [in] 代表故障
  )
  {
    mFaultInfo.set_rep(fault, rep_fault);
    mFsim.set_skip(fault);
  }

  /// @brief 対象の故障に対する支配故障を設定する．
  void
  set_dominator(
    const TpgFault& fault,    ///< [in] 対象の故障
    const TpgFault& dom_fault ///< [in] 支配故障
  )
  {
    mFaultInfo.set_dominator(fault, dom_fault);
    mFsim.set_skip(fault);
  }

  /// @brief 故障シミュレーションの時間を返す．
  double
  fsim_time() const
  {
    return mFsimTimer.get_time();
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションを行う．
  FsimResults
  simulate(
    const std::vector<TestVector>& tv_list, ///< [in] テストパタンのリスト
    std::vector<DPat>& dpat_array           ///< [in] 故障ごとの検出結果を入れる配列
  )
  {
    mFsimTimer.start();
    auto res = mFsim.run_multi(tv_list, true);
    mFsimTimer.stop();

    auto ntv = tv_list.size();
    auto nf = max_fault_size();

    dpat_array.clear();
    dpat_array.reserve(nf);
    for ( SizeType i = 0; i < nf; ++ i ) {
      dpat_array.push_back(DPat(ntv));
    }

    for ( SizeType i = 0; i < ntv; ++ i ) {
      for ( auto fault: res.fault_list(i) ) {
	auto fid = fault.id();
	dpat_array[fid].set(i);
      }
    }

    return res;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の情報
  FaultInfo& mFaultInfo;

  // 故障シミュレータ
  Fsim& mFsim;

  // 故障シミュレータ用のタイマ
  Timer mFsimTimer;

};

END_NAMESPACE_DRUID

#endif // REDMGR_H
