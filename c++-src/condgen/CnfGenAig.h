#ifndef CNFGENAIG_H
#define CNFGENAIG_H

/// @file CnfGenAig.h
/// @brief CnfGenAig のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenFactor.h"
#include "ym/AigHandle.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenAig CnfGenAig.h "CnfGenAig.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenAig :
  public CnfGenFactor
{
public:

  /// @brief コンストラクタ
  CnfGenAig() = default;

  /// @brief デストラクタ
  ~CnfGenAig() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // CnfGen の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr のリストから CNF を作る．
  vector<vector<SatLiteral>>
  expr_to_cnf(
    StructEngine& engine,
    const vector<Expr>& expr_list
  ) override;

  /// @brief Expr のリストから CNF サイズを見積もる．
  CnfSize
  expr_cnf_size(
    const vector<Expr>& expr_list
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief AigHandle に対応する CNF を作る．
  vector<SatLiteral>
  aig_to_cnf(
    StructEngine& engine,
    const AigHandle& aig,
    std::unordered_map<AigHandle, vector<SatLiteral>>& aig_map
  );

  /// @brief 反転したCNFを作る．
  SatLiteral
  invert(
    StructEngine& engine,
    const vector<SatLiteral>& lits
  );

  /// @brief AigHandle に対応する CNF のサイズを見積もる．
  SizeType
  aig_cnf_size(
    const AigHandle& aig,
    CnfSize& size,
    std::unordered_map<AigHandle, SizeType>& aig_map
  );

};

END_NAMESPACE_DRUID

#endif // CNFGENAIG_H
