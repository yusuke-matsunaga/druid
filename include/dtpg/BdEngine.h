#ifndef BDENGINE_H
#define BDENGINE_H

/// @file BdEngine.h
/// @brief BdEngine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "BoolDiffEnc.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BdEngine BdEngine.h "BdEngine.h"
/// @brief BoolDiffEnc を一つ組み込んだ StructEngine
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class BdEngine:
  public StructEngine
{
public:

  /// @brief コンストラクタ
  BdEngine(
    const TpgNetwork& network, ///< [in] ネットワーク
    const TpgNode& node,       ///< [in] 対象のノード
    const JsonValue& option    ///< [in] オプション
    = JsonValue{}
  ) : StructEngine(network, option),
      mBdEnc{new BoolDiffEnc(node, option)}
  {
    add_subenc(std::unique_ptr<SubEnc>{mBdEnc});
  }

  /// @brief デストラクタ
  ~BdEngine() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief root_node() から到達可能な外部出力のリストを返す．
  const TpgNodeList&
  output_list() const
  {
    return mBdEnc->output_list();
  }

  /// @brief root_node() から到達可能な外部出力の数を返す．
  SizeType
  output_num() const
  {
    return mBdEnc->output_num();
  }

  /// @brief root_node() から到達可能な外部出力を返す．
  TpgNode
  output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const
  {
    return mBdEnc->output(pos);
  }

  /// @brief 伝搬変数
  SatLiteral
  prop_var() const
  {
    return mBdEnc->prop_var();
  }

  /// @brief 微分結果を表す変数を返す．
  SatLiteral
  prop_var(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const
  {
    return mBdEnc->prop_var(pos);
  }

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition()
  {
    return mBdEnc->extract_sufficient_condition();
  }

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  )
  {
    return mBdEnc->extract_sufficient_condition(pos);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  BoolDiffEnc* mBdEnc;

};

END_NAMESPACE_DRUID

#endif // BDENGINE_H
