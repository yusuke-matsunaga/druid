#ifndef EXTNAIVE_H
#define EXTNAIVE_H

/// @file ExtNaive.h
/// @brief ExtNaive のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Extractor.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ExtNaive ExtNaive.h "ExtNaive.h"
/// @brief Extractor の実装クラス
//////////////////////////////////////////////////////////////////////
class ExtNaive :
  public Extractor
{
public:

  /// @brief コンストラクタ
  ExtNaive() = default;

  /// @brief デストラクタ
  ~ExtNaive() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // Extractor の継承クラスが定義する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 指定された出力からバックトレースを行う．
  SuffCond
  backtrace(
    const PropGraph& data, ///< [in] 故障伝搬の情報
    const TpgNode& output  ///< [in] 対象の出力ノード
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

};

END_NAMESPACE_DRUID

#endif // EXTNAIVE_H
