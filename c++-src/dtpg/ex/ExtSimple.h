#ifndef EXTSIMPLE_H
#define EXTSIMPLE_H

/// @file ExtSimple.h
/// @brief ExtSimple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Extractor.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExtSimple ExtSimple.h "ExtSimple.h"
/// @brief Extractor の実装クラス
//////////////////////////////////////////////////////////////////////
class ExtSimple :
  public Extractor
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

  /// @brief 制御値を持つ side input を選ぶ．
  std::vector<TpgNode>
  select_cnode(
    const std::vector<std::vector<TpgNode>>& choice_list ///< [in] 候補のノードのリスト
  ) override;

};

END_NAMESPACE_DRUID

#endif // EXTSIMPLE_H
