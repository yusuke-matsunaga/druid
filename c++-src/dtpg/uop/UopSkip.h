#ifndef UOPSKIP_H
#define UOPSKIP_H

/// @file UopSkip.h
/// @brief UopSkip のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "UntestOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class UopSkip UopSkip.h "UopSkip.h"
/// @brief 指定した回数だけ検出不能となったらスキップフラグをセットする．
//////////////////////////////////////////////////////////////////////
class UopSkip :
  public UntestOp
{
public:

  /// @brief コンストラクタ
  UopSkip(
    SizeType threshold  ///< [in] しきい値
  );

  /// @brief デストラクタ
  ~UopSkip();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト不能故障と判定された時の処理
  void
  operator()(
    const TpgFault* f ///< [in] 故障
  );

  /// @brief 検出不能回数とスキップフラグをクリアする．
  void
  clear();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // スキップフラグをセットするしきい値
  SizeType mThreshold;

  // 故障の検出不能回数を保持する辞書
  unordered_map<SizeType, SizeType> mUntestCountMap;

  // 検出不能となった故障の番号を保持するリスト
  vector<SizeType> mUntestList;

  // スキップフラグがセットされた故障の番号を保持するリスト
  vector<SizeType> mSkipList;

};

END_NAMESPACE_DRUID

#endif // UOPSKIP_H
