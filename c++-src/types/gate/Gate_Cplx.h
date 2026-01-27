#ifndef GATE_CPLX_H
#define GATE_CPLX_H

/// @file Gate_Cplx.h
/// @brief Gate_Cplx のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "GateRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Gate_Cplx Gate_Cplx.h "Gate_Cplx.h"
/// @brief 複合ゲート用の GateRep
//////////////////////////////////////////////////////////////////////
class Gate_Cplx :
  public GateRep
{
public:

  /// @brief コンストラクタ
  Gate_Cplx(
    SizeType id,                                ///< [in] ID番号
    const GateType* gate_type,                  ///< [in] ゲートの種類
    const NodeRep* node,                        ///< [in] 出力のノード
    const std::vector<BranchInfo>& branch_info, ///< [in] ブランチの情報のリスト
    FaultType fault_type                        ///< [in] 故障の種類
  ) : GateRep{id, gate_type, fault_type},
      mOutputNode{node},
      mBranchInfoList{branch_info}
  {
  }

  /// @brief デストラクタ
  ~Gate_Cplx() = default;


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

  // 出力のノード
  const NodeRep* mOutputNode;

  // ブランチの情報
  std::vector<BranchInfo> mBranchInfoList;

};

END_NAMESPACE_DRUID

#endif // GATE_CPLX_H
