#ifndef CNFGENCUBE_H
#define CNFGENCUBE_H

/// @file CnfGenCube.h
/// @brief CnfGenCube のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenCube CnfGenCube.h "CnfGenCube.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenCube :
  public CnfGenImpl
{
public:

  /// @brief コンストラクタ
  CnfGenCube() = default;

  /// @brief デストラクタ
  ~CnfGenCube() = default;


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
  /// @return カバーの成り立つ条件を表すリテラルを返す．
  SatLiteral
  cover_to_cnf(
    StructEngine& engine,               ///< [in] StructEngine
    const vector<AssignList>& cube_list ///< [in] カバー（キューブのリスト）
  );

};

END_NAMESPACE_DRUID

#endif // CNFGENCUBE_H
