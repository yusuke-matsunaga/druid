#ifndef EXTSIMPLE_H
#define EXTSIMPLE_H

/// @file ExtSimple.h
/// @brief ExtSimple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ExImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExtSimple ExtSimple.h "ExtSimple.h"
/// @brief ExImpl の実装クラス
//////////////////////////////////////////////////////////////////////
class ExtSimple :
  public ExImpl
{
public:

  /// @brief コンストラクタ
  ExtSimple() = default;

  /// @brief デストラクタ
  ~ExtSimple() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが定義する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の出力を選ぶ．
  const TpgNode*
  select_output() override;

  /// @brief 制御値を持つ side input を選ぶ．
  const TpgNode*
  select_cnode(
    const vector<const TpgNode*>& node_list ///< [in] 候補のノードのリスト
  ) override;

};

END_NAMESPACE_DRUID

#endif // EXTSIMPLE_H
