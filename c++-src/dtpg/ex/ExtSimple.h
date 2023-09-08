#ifndef EXTSIMPLE_H
#define EXTSIMPLE_H

/// @file ExtSimple.h
/// @brief ExtSimple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
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
  ExtSimple(
    const TpgNode* root,    ///< [in] 起点となるノード
    const VidMap& gvar_map, ///< [in] 正常値の変数番号のマップ
    const VidMap& fvar_map, ///< [in] 故障値の変数番号のマップ
    const SatModel& model   ///< [in] SATソルバの作ったモデル
  ) : Extractor{root, gvar_map, fvar_map, model}
  {
  }

  /// @brief デストラクタ
  ~ExtSimple() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが定義する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の出力を選ぶ．
  const TpgNode*
  select_output(
    const vector<const TpgNode*>& output_list ///< [in] 対象の出力ノードのリスト
  ) override;

  /// @brief 制御値を持つ side input を選ぶ．
  const TpgNode*
  select_cnode(
    const vector<const TpgNode*>& node_list ///< [in] 候補のノードのリスト
  ) override;

};

END_NAMESPACE_DRUID

#endif // EXTSIMPLE_H
