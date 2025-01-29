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
  CnfGenNaive() = default;

  /// @brief デストラクタ
  ~CnfGenNaive() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // CnfGenImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件を CNF に変換する．
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,            ///< [in] StructEngine
    const vector<DetCond>& cond_list ///< [in] 検出条件のリスト
  ) override;

  /// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
  CnfSize
  calc_cnf_size(
    const vector<DetCond>& cond_list ///< [in] カバー（キューブのリスト）
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief カバーをCNFに変換する．
  static
  SatLiteral
  cover_to_cnf(
    StructEngine& engine,
    const vector<AssignList>& cube_list
  );

};

END_NAMESPACE_DRUID

#endif // CNFGENNAIVE_H
