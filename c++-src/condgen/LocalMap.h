#ifndef LOCALMAP_H
#define LOCALMAP_H

/// @file LocalMap.h
/// @brief LocalMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SopCover.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class LocalMap LocalMap.h "LocalMap.h"
/// @brief 現れる変数のみをパックした辞書
///
/// 間にすき間の空いた変数で構成されたカバーをパックした SopCover に
/// 変換する処理とその結果の Expr を元の変数に remap する処理を提供する．
//////////////////////////////////////////////////////////////////////
class LocalMap
{
public:

  /// @brief コンストラクタ
  LocalMap() = default;

  /// @brief デストラクタ
  ~LocalMap() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief カバーを表す Expr から SopCover を作る．
  ///
  /// expr に現れる変数を登録する．
  SopCover
  to_cover(
    const std::vector<std::vector<Literal>>& literal_list
  )
  {
    std::vector<std::vector<Literal>> local_literal_list;
    local_literal_list.reserve(literal_list.size());
    for ( auto src_lits: literal_list ) {
      std::vector<Literal> lits;
      lits.reserve(src_lits.size());
      for ( auto src_lit: src_lits ) {
	auto lit = to_literal(src_lit);
	lits.push_back(lit);
      }
      local_literal_list.push_back(lits);
    }
    auto nv = local_num();
    auto cover = SopCover(nv, local_literal_list);
    return cover;
  }

  /// @brief ローカルIDを用いた Expr を元の ID を用いた Expr に変換する．
  Expr
  remap_expr(
    const Expr& local_expr ///< [in] ローカルな Expr
  ) const
  {
    Expr::VarMap var_map;
    for ( SizeType local_id = 0;
	  local_id < local_num();
	  ++ local_id ) {
      auto id = get_id(local_id);
      var_map.emplace(local_id, id);
    }
    auto expr = local_expr.remap_var(var_map);
    return expr;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューブを表す Expr を変換する．
  std::vector<Literal>
  to_cube(
    const Expr& expr
  )
  {
    if ( expr.is_literal() ) {
      auto lit = to_literal(expr.literal());
      return {lit};
    }
    if ( expr.is_and() ) {
      std::vector<Literal> lits;
      lits.reserve(expr.operand_num());
      for ( auto& expr1: expr.operand_list() ) {
	auto lit = to_literal(expr1.literal());
	lits.push_back(lit);
      }
      return lits;
    }
    throw std::invalid_argument{"expr is not a cube"};
    return {};
  }

  /// @brief リテラルをローカルなリテラルに変換する．
  Literal
  to_literal(
    Literal lit
  )
  {
    auto varid = lit.varid();
    auto local_id = reg_id(varid);
    auto inv = lit.is_negative();
    return Literal(local_id, inv);
  }

  /// @brief ID を登録する．
  /// @return ローカルIDを返す．
  SizeType
  reg_id(
    SizeType id ///< [in] ID番号
  )
  {
    if ( mLocalMap.count(id) == 0 ) {
      auto local_id = mIdList.size();
      mIdList.push_back(id);
      mLocalMap.emplace(id, local_id);
      return local_id;
    }
    return mLocalMap.at(id);
  }

  /// @brief ローカルIDを返す．
  ///
  /// - ID が登録されていない場合は例外を送出する．
  SizeType
  local_id(
    SizeType id ///< [in] ID番号
  ) const
  {
    if ( mLocalMap.count(id) == 0 ) {
      throw std::invalid_argument{"id is not registered"};
    }
    return mLocalMap.at(id);
  }

  /// @brief 登録されているIDの数を返す．
  SizeType
  local_num() const
  {
    return mIdList.size();
  }

  /// @brief ローカルIDからもとのIDを返す．
  ///
  /// ローカルIDが範囲外の場合は例外を送出する．
  SizeType
  get_id(
    SizeType local_id ///< [in] ローカルID
  ) const
  {
    if ( local_id >= local_num() ) {
      throw std::out_of_range{" local_id is out of range"};
    }
    return mIdList[local_id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID をキーにしてローカルIDを記録する辞書
  std::unordered_map<SizeType, SizeType> mLocalMap;

  // ローカルIDのリスト
  std::vector<SizeType> mIdList;

};

END_NAMESPACE_DRUID

#endif // LOCALMAP_H
