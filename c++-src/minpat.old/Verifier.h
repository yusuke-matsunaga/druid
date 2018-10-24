﻿#ifndef VERIFIER_H
#define VERIFIER_H

/// @file Verifier.h
/// @brief Verifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2014 Yusuke Matsunaga
/// All rights reserved.

#include "sa/sa_nsdef.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
/// @class Verifier Verifier.h "Verifier.h"
/// @brief 圧縮したパタンが正しいかチェックするクラス
//////////////////////////////////////////////////////////////////////
class Verifier
{
public:

  /// @brief コンストラクタ
  Verifier();

  /// @brief デストラクタ
  ~Verifier();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 与えられたパタンリストで全検出済み故障を検出できるか調べる．
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] fault_list 故障のリスト
  /// @param[in] pat_list パタンのリスト
  bool
  check(Fsim& fsim,
	const vector<const TpgFault*>& fault_list,
	const vector<const TestVector*>& pat_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM_SATPG_SA

#endif // VERIFIER_H
