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
//#include "types/TpgFaultList.h"
#include "types/PackedVal.h"
#include "fsim/Fsim.h"
#include "FaultInfo.h"
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
    std::vector<PackedVal>& dpat_array      ///< [in] 故障ごとの検出結果を入れる配列
  )
  {
    mFsimTimer.start();
    auto res = mFsim.run_multi(tv_list, true);
    mFsimTimer.stop();

    dpat_array.clear();
    dpat_array.resize(max_fault_size(), PV_ALL0);

    auto ntv = res.tv_num();
    for ( SizeType i = 0; i < ntv; ++ i ) {
      PackedVal bit = 1ULL << i;
      for ( auto fault: res.fault_list(i) ) {
	auto fid = fault.id();
	dpat_array[fid] |= bit;
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
