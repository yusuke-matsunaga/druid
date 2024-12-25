#ifndef CNFGENIMPL_H
#define CNFGENIMPL_H

/// @file CnfGenImpl.h
/// @brief CnfGenImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "DetCond.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenImpl CnfGenImpl.h "CnfGenImpl.h"
/// @brief CnfGen の下請けクラスの基底クラス
//////////////////////////////////////////////////////////////////////
class CnfGenImpl
{
public:

  /// @brief コンストラクタ
  explicit
  CnfGenImpl(
    StructEngine& engine ///< [in] StructEngine
  ) : mEngine{engine}
  {
  }

  /// @brief デストラクタ
  virtual
  ~CnfGenImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件を CNF に変換する．
  vector<SatLiteral>
  make_cnf(
    const DetCond& cond ///< [in] 検出条件
  );

  /// @brief 条件を CNF に変換した際の項数とリテラル数を計算する．
  CnfSize
  calc_cnf_size(
    const DetCond& cond ///< [in] 検出条件
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューブを表すリテラルのリストを返す．
  vector<SatLiteral>
  cube_to_literals(
    const AssignList& cube ///< [in] キューブ
  );

  /// @brief StructEngine を返す．
  StructEngine&
  engine()
  {
    return mEngine;
  }

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mEngine.solver();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief カバーをCNFに変換する．
  /// @return カバーの成り立つ条件を表すリテラルを返す．
  virtual
  SatLiteral
  cover_to_cnf(
    const vector<AssignList>& cube_list ///< [in] カバー（キューブのリスト）
  ) = 0;

  /// @brief カバーをCNFに変換した時の CNF のサイズを見積もる．
  virtual
  CnfSize
  calc_cover_size(
    const vector<AssignList>& cube_list ///< [in] カバー（キューブのリスト）
  ) = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // StructEngine
  StructEngine& mEngine;

};

END_NAMESPACE_DRUID

#endif // CNFGENIMPL_H
