#ifndef FAULT_EX_H
#define FAULT_EX_H

/// @file Fault_Ex.h
/// @brief Fault_Ex のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "FaultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Fault_Ex Fault_Ex.h "Fault_Ex.h"
/// @brief ゲート網羅故障を表すクラス
//////////////////////////////////////////////////////////////////////
class Fault_Ex :
  public FaultRep
{
public:

  /// @brief コンストラクタ
  Fault_Ex(
    SizeType id,                   ///< [in] ID番号
    const GateRep* gate,           ///< [in] 対象のゲート
    const std::vector<bool>& ivals ///< [in] 入力の値のリスト
  ) : FaultRep{id, gate},
      mIvals{ivals}
  {
  }

  /// @brief デストラクタ
  ~Fault_Ex() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

  /// @brief 故障値を返す．
  ///
  /// 網羅故障の場合は意味を持たない．
  Fval2
  fval() const override;

  /// @brief 故障伝搬の起点となるノードを返す．
  const NodeRep*
  origin_node() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力値のリスト
  std::vector<bool> mIvals;

};

END_NAMESPACE_DRUID

#endif // FAULT_EX_H
