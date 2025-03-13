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
#include "CondGenMgr.h"
#include "TpgNode.h"
#include "ym/AigHandle.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGen CnfGen.h "CnfGen.h"
/// @brief CNF の生成を行う．
//////////////////////////////////////////////////////////////////////
class CnfGen
{
public:

  using CondLits = CondGenMgr::CondLits;

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
  /// - aig_list と bd_list のORが条件となる．
  /// - ただし AigInfo も BdInfo も空の場合がある．
  static
  std::vector<CondLits>
  make_cnf(
    StructEngine& engine,                     ///< [in] 基本エンジン
    const std::vector<AigHandle>& aig_list,   ///< [in] 論理式を表す AIG のリスト
    const std::vector<SizeType>& ffr_id_list, ///< [in] aig_list の各要素に対応するFFR番号のリスト
    const std::vector<BdInfo>& bd_list        ///< [in] 故障伝搬条件を作る出力のリスト
  );

};

END_NAMESPACE_DRUID

#endif // CNFGEN_H
