#ifndef UOPSKIP_H
#define UOPSKIP_H

/// @file UopSkip.h
/// @brief UopSkip のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
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
  /// @param[in] threshold しきい値
  /// @param[in] max_fault_id 故障番号の最大値
  UopSkip(int threshold,
	  int max_fault_id);

  /// @brief デストラクタ
  virtual
  ~UopSkip();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト不能故障と判定された時の処理
  /// @param[in] f 故障
  virtual
  void
  operator()(const TpgFault* f);

  /// @brief 検出不能回数とスキップフラグをクリアする．
  void
  clear();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // スキップフラグをセットするしきい値
  int mThreshold;

  // 故障の検出不能回数を保持する配列
  vector<int> mUntestCountArray;

  // 検出不能となった故障の番号を保持するリスト
  vector<int> mUntestList;

  // スキップフラグがセットされた故障の番号を保持するリスト
  vector<int> mSkipList;

};

END_NAMESPACE_DRUID

#endif // UOPSKIP_H
