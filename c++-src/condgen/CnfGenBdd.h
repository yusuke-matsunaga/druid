#ifndef CNFGENBDD_H
#define CNFGENBDD_H

/// @file CnfGenBdd.h
/// @brief CnfGenBdd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenImpl.h"
#include "ym/BddMgr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenBdd CnfGenBdd.h "CnfGenBdd.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenBdd :
  public CnfGenImpl
{
public:

  /// @brief コンストラクタ
  CnfGenBdd(
    StructEngine& engine, ///< [in] StructEngine
    SizeType size_limit   ///< [in] BDD サイズの上限値
  ) : CnfGenImpl{engine},
      mSizeLimit{size_limit}
  {
  }

  /// @brief デストラクタ
  ~CnfGenBdd() = default;


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


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief カバーを BDD のリストに変換する．
  vector<Bdd>
  cover_to_bdd(
    const vector<AssignList>& cube_list ///< [in] カバー（キューブのリスト）
  );

  /// @brief キューブをBDDに変換する．
  Bdd
  cube_to_bdd(
    const AssignList& cube ///< [in] キューブ
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // BDD マネージャ
  BddMgr mBddMgr;

  // BDDサイズの上限値
  SizeType mSizeLimit;

};

END_NAMESPACE_DRUID

#endif // CNFGENBDD_H
