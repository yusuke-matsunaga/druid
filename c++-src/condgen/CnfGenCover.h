#ifndef CNFGENCOVER_H
#define CNFGENCOVER_H

/// @file CnfGenCover.h
/// @brief CnfGenCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenCover CnfGenCover.h "CnfGenCover.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenCover :
  public CnfGenImpl
{
public:

  /// @brief コンストラクタ
  CnfGenCover() = default;

  /// @brief デストラクタ
  ~CnfGenCover() = default;


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

#endif // CNFGENCOVER_H
