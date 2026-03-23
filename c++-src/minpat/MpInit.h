#ifndef MPINIT_H
#define MPINIT_H

/// @file MpInit.h
/// @brief MpInit のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpInit MpInit.h "MpInit.h"
/// @brief 最小テストパタン生成用の初期解を作るクラス
///
/// コンストラクタでオブジェクトを作った瞬間に動作は終わる．
/// ほぼ DtpgMgr の出力結果を変換しただけ
//////////////////////////////////////////////////////////////////////
class MpInit
{
public:

  /// @brief コンストラクタ
  MpInit(
    const TpgNetwork& network,      ///< [in] 対象のネットワーク
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option = {}  ///< [in] オプション
  );

  /// @brief デストラクタ
  ~MpInit() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークを得る．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 検出された故障リストを得る．
  const TpgFaultList&
  det_fault_list() const
  {
    return mDetFaultList;
  }

  /// @brief テスト不能の故障リストを得る．
  const TpgFaultList&
  untest_fault_list() const
  {
    return mUntestFaultList;
  }

  /// @brief 未検出の故障リストを得る．
  const TpgFaultList&
  undet_fault_list() const
  {
    return mUndetFaultList;
  }

  /// @brief det_fault_list() 中の位置を返す．
  SizeType
  find(
    const TpgFault& fault ///< [in] 対象の故障
  ) const
  {
    return mFidMap.at(fault.id());
  }

  /// @brief テストベクタのリストを返す．
  ///
  /// @code
  /// MpInit init(network, fault_list, option);
  ///
  /// for ( auto fault: init.det_fault_list() ) {
  ///   auto pos = init.find(fault);
  ///   auto tv = init.tv_list()[pos];
  ///   // tv は fault を検出するテストパタン
  /// }
  /// @endcode
  const std::vector<TestVector>
  tv_list() const
  {
    return mTvList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 検出された故障を追加する．
  void
  add_det_fault(
    const TpgFault& fault ///< [in] 故障
  )
  {
    auto pos = mDetFaultList.size();
    mDetFaultList.push_back(fault);
    mFidMap.emplace(fault.id(), pos);
  }

  /// @brief テスト不能故障を追加する．
  void
  add_untest_fault(
    const TpgFault& fault ///< [in] 故障
  )
  {
    mUntestFaultList.push_back(fault);
  }

  /// @brief 未検出故障を追加する．
  void
  add_undet_fault(
    const TpgFault& fault ///< [in] 故障
  )
  {
    mUndetFaultList.push_back(fault);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  TpgNetwork mNetwork;

  // 検出された故障のリスト
  TpgFaultList mDetFaultList;

  // 故障番号をキーとして mDetFaultList 中の位置を格納した辞書
  std::unordered_map<SizeType, SizeType> mFidMap;

  // テスト不能故障のリスト
  TpgFaultList mUntestFaultList;

  // 未検出の故障のリスト
  TpgFaultList mUndetFaultList;

  // テストパタンのリスト
  std::vector<TestVector> mTvList;

};

END_NAMESPACE_DRUID

#endif // MPINIT_H
