﻿#ifndef MINPATSIMPLE_H
#define MINPATSIMPLE_H

/// @file MinPatSimple.h
/// @brief MinPatSimple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatNaive.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
/// @class MinPatSimple MinPatSimple.h "MinPatSimple.h"
/// @brief MinPatNaive の故障の順番を改良したクラス
//////////////////////////////////////////////////////////////////////
class MinPatSimple :
  public MinPatNaive
{
public:

  /// @brief コンストラクタ
  /// @param[in] group_dominance グループ支配を計算する．
  /// @param[in] rep_faults 等価故障の検出を行う．
  MinPatSimple(bool group_dominance,
	       bool rep_faults);

  /// @brief デストラクタ
  virtual
  ~MinPatSimple();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  /// @param[in] fid_list 検出された故障番号のリスト
  /// @param[in] tvmgr テストベクタマネージャ
  /// @param[in] fsim2 2値の故障シミュレータ(検証用)
  virtual
  void
  init(const vector<ymuint>& fid_list,
       TvMgr& tvmgr,
       Fsim& fsim2);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 等価故障の検出を行うとき true にするフラグ
  bool mRepFaults;

};

END_NAMESPACE_YM_SATPG_SA

#endif // MINPATSIMPLE_H
