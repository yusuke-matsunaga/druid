#ifndef BD2ENGINE_H
#define BD2ENGINE_H

/// @file Bd2Engine.h
/// @brief Bd2Engine のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/StructEngine.h"
#include "dtpg/SuffCond.h"
#include "types/TpgNode.h"


BEGIN_NAMESPACE_DRUID

class BoolDiffEnc;

//////////////////////////////////////////////////////////////////////
/// @class Bd2Engine Bd2Engine.h "dtpg/Bd2Engine.h"
/// @brief BoolDiffEnc を２つ組み込んだ StructEngine
/// @ingroup DtpgGroup
//////////////////////////////////////////////////////////////////////
class Bd2Engine:
  public StructEngine
{
public:

  /// @brief コンストラクタ
  Bd2Engine(
    const TpgNode& node1,          ///< [in] 対象のノード1
    const TpgNode& node2,          ///< [in] 対象のノード2
    const ConfigParam& option = {} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~Bd2Engine() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief root_node1() から到達可能な外部出力のリストを返す．
  const TpgNodeList&
  output_list1() const;

  /// @brief root_node1() から到達可能な外部出力の数を返す．
  SizeType
  output_num1() const;

  /// @brief root_node1() から到達可能な外部出力を返す．
  TpgNode
  output1(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num1() )
  ) const;

  /// @brief root_node1() の伝搬変数
  SatLiteral
  prop_var1() const;

  /// @brief root_node1() の微分結果を表す変数を返す．
  SatLiteral
  prop_var1(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num1() )
  ) const;

  /// @brief root_node2() から到達可能な外部出力のリストを返す．
  const TpgNodeList&
  output_list2() const;

  /// @brief root_node2() から到達可能な外部出力の数を返す．
  SizeType
  output_num2() const;

  /// @brief root_node2() から到達可能な外部出力を返す．
  TpgNode
  output2(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num2() )
  ) const;

  /// @brief root_node2() の伝搬変数
  SatLiteral
  prop_var2() const;

  /// @brief root_node2() の微分結果を表す変数を返す．
  SatLiteral
  prop_var2(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num2() )
  ) const;

  /// @brief SAT問題の解から十分条件を求める．
  SuffCond
  extract_sufficient_condition(
    const SatModel& model,             ///< [in] SAT問題の解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );

  /// @brief SAT問題の解から十分条件を求める．
  SuffCond
  extract_sufficient_condition(
    SizeType pos,                      ///< [in] 出力番号 ( 0 <= pos < output_num() )
    const SatModel& model,             ///< [in] SAT問題の解
    const AssignList& assign_list = {} ///< [in] 事前の値割り当て
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  BoolDiffEnc* mBdEnc1;

  BoolDiffEnc* mBdEnc2;

};

END_NAMESPACE_DRUID

#endif // BD2ENGINE_H
