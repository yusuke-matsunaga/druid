#ifndef CNFGENFACTOR_H
#define CNFGENFACTOR_H

/// @file CnfGenFactor.h
/// @brief CnfGenFactor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenFactor CnfGenFactor.h "CnfGenFactor.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenFactor :
  public CnfGenImpl
{
public:

  /// @brief コンストラクタ
  CnfGenFactor() = default;

  /// @brief デストラクタ
  ~CnfGenFactor() = default;


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

#endif // CNFGENFACTOR_H
