#ifndef CNFGEN_H
#define CNFGEN_H

/// @file CnfGen.h
/// @brief CnfGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "DetCond.h"
#include "AssignList.h"
#include "Expr2Cnf.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGen CnfGen.h "CnfGen.h"
/// @brief CnfGenMgr の下請けクラスの基底クラス
//////////////////////////////////////////////////////////////////////
class CnfGen
{
public:

  /// @brief コンストラクタ
  CnfGen() = default;

  /// @brief デストラクタ
  virtual
  ~CnfGen() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件を CNF に変換する．
  vector<vector<SatLiteral>>
  make_cnf(
    StructEngine& engine,            ///< [in] StructEngine
    const vector<DetCond>& cond_list ///< [in] 検出条件のリスト
  );

  /// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
  CnfSize
  calc_cnf_size(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    const vector<DetCond>& cond_list ///< [in] カバー（キューブのリスト）
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief DetCond から Expr を作る．
  virtual
  Expr
  cond_to_expr(
    const DetCond::CondData& cond
  ) = 0;

  /// @brief Expr のリストから CNF を作る．
  virtual
  vector<vector<SatLiteral>>
  expr_to_cnf(
    StructEngine& engine,
    const vector<Expr>& expr_list
  ) = 0;

  /// @brief Expr のリストから CNF サイズを見積もる．
  virtual
  CnfSize
  expr_cnf_size(
    const vector<Expr>& expr_list
  ) = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件を Expr に変換する．
  vector<Expr>
  _make_expr_list(
    const vector<DetCond>& cond_list
  );

};

END_NAMESPACE_DRUID

#endif // CNFGEN_H
