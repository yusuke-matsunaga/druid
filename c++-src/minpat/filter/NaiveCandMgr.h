#ifndef NAIVECANDMGR_H
#define NAIVECANDMGR_H

/// @file NaiveCandMgr.h
/// @brief NaiveCandMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "CandMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NaiveCandMgr NaiveCandMgr.h "NaiveCandMgr.h"
/// @brief 単純な CandMgr
//////////////////////////////////////////////////////////////////////
class NaiveCandMgr :
  public CandMgr
{
public:

  /// @brief コンストラクタ
  NaiveCandMgr(
    const FaultInfo& fault_info ///< [in] 対象の故障の情報
  );

  /// @brief デストラクタ
  ~NaiveCandMgr();


private:
  //////////////////////////////////////////////////////////////////////
  // Filter の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  bool
  update(
    const std::vector<PackedVal>& dpat_array ///< [in] 故障の検出状況のピットパタン
  ) override;

  /// @brief 終了処理
  EqDomCand
  end() override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価な可能性のある故障のリストを返す．
  TpgFaultList
  eqcand_list(
    const TpgFault& fault
  ) const;

  /// 故障対に対するインデックスを計算する．
  SizeType
  _index(
    const TpgFault& fault1,
    const TpgFault& fault2
  ) const
  {
    return fault1.id() * mSize + fault2.id();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障番号の最大値
  SizeType mSize;

  // 対象の故障リスト
  TpgFaultList mFaultList;

  // 2つの故障間の関係を表すビットパタンの配列
  // サイズは mSize * mSize
  // 0: 故障1が故障2を支配する可能性なし(1, 0 のパタンあり)
  std::vector<bool> mArray;

  // 支配する故障候補のリストの配列
  // サイズは mSize
  std::vector<TpgFaultList> mDomCandListArray;

  // mDomCandListArray が初期化されていたら true となるフラグ
  bool mInitialized{false};

};

END_NAMESPACE_DRUID

#endif // NAIVECANDMGR_H
