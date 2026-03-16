#ifndef EXTSTD_H
#define EXTSTD_H

/// @file ExtStd.h
/// @brief ExtStd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Extractor.h"
#include "ym/Bdd.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExtStd ExtStd.h "ExtStd.h"
/// @brief Extractor の実装クラス
//////////////////////////////////////////////////////////////////////
class ExtStd :
  public Extractor
{
public:

  /// @brief コンストラクタ
  ExtStd() = default;

  /// @brief デストラクタ
  ~ExtStd() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // Extractor の継承クラスが定義する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 指定された出力からバックトレースを行う．
  SuffCond
  backtrace(
    const ExData& data,   ///< [in] 故障伝搬の情報
    const TpgNode& output ///< [in] 対象の出力ノード
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 制御値を持つ side input を選ぶ．
  std::vector<TpgNode>
  select_cnode(
    const std::vector<std::vector<TpgNode>>& choice_list ///< [in] 候補のノードのリスト
  );

  /// @brief 境界部分を入力として出力のBDDを作る．
  Bdd
  make_bdd(
    const TpgNode& node,                        ///< [in] 対象のノード
    std::unordered_map<SizeType, Bdd>& bdd_dict ///< [in] ノードに対応するBDDの辞書
  );

};

END_NAMESPACE_DRUID

#endif // EXTSTD_H
