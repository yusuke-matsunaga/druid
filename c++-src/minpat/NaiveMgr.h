#ifndef NAIVEMGR_H
#define NAIVEMGR_H

/// @file NaiveMgr.h
/// @brief NaiveMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "fsim/FsimResults.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NaiveMgr NaiveMgr.h "NaiveMgr.h"
/// @brief 故障シミュレーションの結果で故障グループの分割を行うクラス
//////////////////////////////////////////////////////////////////////
class NaiveMgr
{
public:

  /// @brief コンストラクタ
  NaiveMgr(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    SizeType size                   ///< [in] 最大サイズ
  ) : mFaultList{fault_list},
      mSize{size},
      mArray(mSize * mSize, false),
      mDomCandListArray(mSize)
  {
  }

  /// @brief デストラクタ
  ~NaiveMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションの結果を登録する．
  /// @return 変化があったら true を返す．
  bool
  add(
    const FsimResults& res ///< [in] 故障シミュレーションの結果
  );

  /// @brief 等価な可能性のある故障のリストを返す．
  TpgFaultList
  eqcand_list(
    const TpgFault& fault
  ) const;

  /// @brief 支配する可能性のある故障のリストを返す．
  TpgFaultList
  domcand_list(
    const TpgFault& fault
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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

  // 対象の故障のリスト
  TpgFaultList mFaultList;

  // サイズ
  SizeType mSize;

  // 2つの故障間の関係を表すビットパタンの配列
  // サイズは mSize * mSize
  // 0: 故障1が故障2を支配する可能性なし(1, 0 のパタンあり)
  std::vector<bool> mArray;

  // 支配する故障候補のリストの配列
  std::vector<TpgFaultList> mDomCandListArray;

  // mDomCandListArray が初期化されていたら true となるフラグ
  bool mInitialized{false};


};

END_NAMESPACE_DRUID

#endif // NAIVEMGR_H
