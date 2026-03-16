#ifndef SUFFCOND_H
#define SUFFCOND_H

/// @file SuffCond.h
/// @brief SuffCond のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/AssignList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class SuffCond SuffCond.h "SuffCond.h"
/// @brief 故障検出の十分条件を表すクラス
///
/// 2つの AssignList を持つだけのクラス．
/// いわゆる Python の named tuple 的な使い方を想定している．
///
/// - main_cond: 真の条件
/// - aux_cond:  補助的な条件．実際には必要ではないが，テストベクタを
///              作るときには考慮しないと故障シミュレータが正しく
///              動かない．
//////////////////////////////////////////////////////////////////////
class SuffCond
{
public:

  /// @brief 空のコンストラクタ
  SuffCond() = default;

  /// @brief 値を指定したコンストラクタ
  SuffCond(
    const AssignList& main_cond, ///< [in] 真の条件
    const AssignList& aux_cond   ///< [in] 補助的な条件
  ) : mMainCond(main_cond),
      mAuxCond(aux_cond)
  {
  }

  /// @brief デストラクタ
  ~SuffCond() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 真の条件を返す．
  const AssignList&
  main_cond() const
  {
    return mMainCond;
  }

  /// @brief 補助的な条件を返す．
  const AssignList&
  aux_cond() const
  {
    return mAuxCond;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 真の条件
  AssignList mMainCond;

  // 補助的な条件
  AssignList mAuxCond;

};

END_NAMESPACE_DRUID

#endif // SUFFCOND_H
