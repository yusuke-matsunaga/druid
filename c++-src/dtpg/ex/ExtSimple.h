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

};

END_NAMESPACE_DRUID

#endif // EXTSIMPLE_H
