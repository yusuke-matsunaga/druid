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
/// @brief CnfGen の下請けクラスの基底クラス
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
    const vector<DetCond>& cond_list ///< [in] カバー（キューブのリスト）
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @breif cond_list の cube_list() の内容をリテラルのリストに変換する．
  vector<vector<Literal>>
  to_literal_list(
    const DetCond& cond ///< [in] 条件
  )
  {
    vector<vector<Literal>> literal_list;
    literal_list.reserve(cond.cube_list().size());
    for ( auto& cube: cond.cube_list() ) {
      vector<Literal> cube_lits;
      cube_lits.reserve(cube.size());
      for ( auto& as: cube ) {
	reg_assign(as);
	auto lit = literal(as);
	cube_lits.push_back(lit);
      }
      literal_list.push_back(cube_lits);
    }
    return literal_list;
  }

  /// @brief リテラルのリストから Expr を作る．
  virtual
  Expr
  to_expr(
    const vector<vector<Literal>>& literal_list
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

  /// @brief 変数の数を得る．
  SizeType
  var_num() const
  {
    return mAssignList.size();
  }

  /// @brief 変数番号を得る．
  ///
  /// - assign が登録されていなければ例外を送出する．
  /// - assign.val() は無視される．
  SizeType
  varid(
    const Assign& assign ///< [in] 値の割当
  ) const
  {
    auto key = make_key(assign);
    if ( mMap.count(key) == 0 ) {
      throw std::invalid_argument{"assign is not registered"};
    }
    auto id = mMap.at(key);
    return id;
  }

  /// @brief リテラルに変換する．
  ///
  /// - assign が登録されていなければ例外を送出する．
  Literal
  literal(
    const Assign& assign ///< [in] 値の割当
  ) const
  {
    auto id = varid(assign);
    bool inv = assign.val() == false;
    return Literal(id, inv);
  }

  /// @brief リテラルから元の割り当てを得る．
  Assign
  get_assign(
    Literal lit ///< [in] リテラル
  ) const
  {
    auto vid = lit.varid();
    auto as = get_assign(vid);
    if ( lit.is_negative() ) {
      as = ~as;
    }
    return as;
  }

  /// @brief 変数番号から元の割当を得る．
  Assign
  get_assign(
    SizeType id ///< [in] 変数番号 ( 0 <= id < var_num() )
  ) const
  {
    if ( id >= var_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mAssignList[id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件を Expr に変換する．
  vector<Expr>
  _make_expr_list(
    const vector<DetCond>& cond_list
  )
  {
    vector<Expr> expr_list;
    expr_list.reserve(cond_list.size());
    for ( auto& cond: cond_list ) {
      if ( cond.type() == DetCond::Detected ) {
	auto literal_list = to_literal_list(cond);
	auto expr = to_expr(literal_list);
	expr_list.push_back(expr);
      }
      else {
	expr_list.push_back(Expr::zero());
      }
    }
    return expr_list;
  }

  /// @brief 変数を登録する．
  /// @return ID 番号を返す．
  ///
  /// - すでに登録されていたらなにもしない．
  /// - Assign.val() は無視される．
  SizeType
  reg_assign(
    const Assign& assign ///< [in] 値の割当
  )
  {
    auto key = make_key(assign);
    if ( mMap.count(key) == 0 ) {
      auto id = var_num();
      mMap.emplace(key, id);
      auto posi_assign = assign.val() ? assign : ~assign;
      mAssignList.push_back(posi_assign);
      return id;
    }
    return mMap.at(key);
  }

  /// @brief Assign からキーを作る．
  ///
  /// Assign.val() は無視される．
  static
  SizeType
  make_key(
    const Assign& assign ///< [in] 値の割当
  )
  {
    return assign.node()->id() * 2 + assign.time();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Assign のノード(と時間)をキーとして変数番号を格納する連想配列
  std::unordered_map<SizeType, SizeType> mMap;

  // 変数番号をキーにして Assign を格納する配列
  std::vector<Assign> mAssignList;

};

END_NAMESPACE_DRUID

#endif // CNFGEN_H
