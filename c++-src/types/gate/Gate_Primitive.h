#ifndef GATE_PRIMITIVE_H
#define GATE_PRIMITIVE_H

/// @file Gate_Primitive.h
/// @brief Gate_Primitive の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "GateRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Gate_Primitive Gate_Primitive.h "Gate_Primitive.h"
/// @brief TpgNode そのものに対応する GateRep の派生クラス
//////////////////////////////////////////////////////////////////////
class Gate_Primitive :
  public GateRep
{
public:

  /// @brief コンストラクタ
  Gate_Primitive(
    SizeType id,               ///< [in] ID番号
    const GateType* gate_type, ///< [in] ゲートの種類
    const NodeRep* node,       ///< [in] ノード
    FaultType fault_type       ///< [in] 故障の種類
  ) : GateRep{id, gate_type, fault_type},
      mNode{node}
  {
  }

  /// @brief デストラクタ
  ~Gate_Primitive() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力に対応するノードを返す．
  const NodeRep*
  output_node() const override;

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief ブランチの情報を返す．
  BranchInfo
  branch_info(
    SizeType pos ///< [in] 入力位置
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実際のノード
  const NodeRep* mNode;

};

END_NAMESPACE_DRUID

#endif // GATE_PRIMITIVE_H
