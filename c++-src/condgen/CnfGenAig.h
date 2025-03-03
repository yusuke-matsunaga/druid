#ifndef CNFGENAIG_H
#define CNFGENAIG_H

/// @file CnfGenAig.h
/// @brief CnfGenAig のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenAig CnfGenAig.h "CnfGenAig.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenAig :
  public CnfGenImpl
{
public:

  /// @brief コンストラクタ
  CnfGenAig() = default;

  /// @brief デストラクタ
  ~CnfGenAig() = default;


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

};

END_NAMESPACE_DRUID

#endif // CNFGENAIG_H
