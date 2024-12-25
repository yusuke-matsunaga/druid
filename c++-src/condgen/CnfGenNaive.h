#ifndef CNFGENNAIVE_H
#define CNFGENNAIVE_H

/// @file CnfGenNaive.h
/// @brief CnfGenNaive のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenNaive CnfGenNaive.h "CnfGenNaive.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenNaive :
  public CnfGenImpl
{
public:

  /// @brief コンストラクタ
  CnfGenNaive(
    StructEngine& engine ///< [in] StructEngine
  ) : CnfGenImpl{engine}
  {
  }

  /// @brief デストラクタ
  ~CnfGenNaive() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // CnfGenImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief カバーをCNFに変換する．
  /// @return カバーの成り立つ条件を表すリテラルを返す．
  SatLiteral
  cover_to_cnf(
    const vector<AssignList>& cube_list ///< [in] カバー（キューブのリスト）
  ) override;

  /// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
  CnfSize
  calc_cover_size(
    const vector<AssignList>& cube_list ///< [in] カバー（キューブのリスト）
  ) override;

};

END_NAMESPACE_DRUID

#endif // CNFGENNAIVE_H
