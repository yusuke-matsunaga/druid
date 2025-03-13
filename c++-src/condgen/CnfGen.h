#ifndef CNFGEN_H
#define CNFGEN_H

/// @file CnfGen.h
/// @brief CnfGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGen CnfGen.h "CnfGen.h"
/// @brief CNF の生成を行う．
//////////////////////////////////////////////////////////////////////
class CnfGen
{
public:

  /// @brief ブール微分用の情報
  struct BdInfo {
    SizeType id;                             ///< FFR番号
    const TpgNode* root;                     ///< 起点のノード
    std::vector<const TpgNode*> output_list; ///< 出力のリスト
  };


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief CNFを生成する．
  /// @return 各要素ごとの条件を表すリテラルのリストの配列を返す．
  ///
  /// - aig_array[i] | bd_array[i] が条件となる．
  /// - ただし AIG も BdInfo も空の場合がある．
  static
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,                    ///< [in] 基本エンジン
    const std::vector<AigHandle>& aig_array, ///< [in] 論理式を表す AIG のリスト
    const std::vector<BdInfo>& bd_array      ///< [in] 故障伝搬条件を作る出力のリスト
  );

};

END_NAMESPACE_DRUID

#endif // CNFGEN_H
