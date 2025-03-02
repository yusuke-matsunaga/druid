#ifndef ASSIGNVARDICT_H
#define ASSIGNVARDICT_H

/// @file AssignVarDict.h
/// @brief AssignVarDict のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Assign.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AssignVarDict AssignVarDict.h "AssignVarDict.h"
/// @brief Assign のノードと変数の対応を取るためのクラス
//////////////////////////////////////////////////////////////////////
class AssignVarDict
{
public:

  /// @brief コンストラクタ
  AssignVarDict() = default;

  /// @brief デストラクタ
  ~AssignVarDict() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief AssignList のリストをカバーに変換する．
  SopCover
  to_cover(
    const vector<AssignList>& cube_list
  )
  {
    vector<vector<Literal>> literal_list;
    literal_list.reserve(cube_list.size());
    for ( auto& cube: cube_list ) {
      vector<Literal> cube_lits;
      cube_lits.reserve(cube.size());
      for ( auto& as: cube ) {
	auto id = reg_assign(as);
	auto lit = to_literal(as);
	cube_lits.push_back(lit);
      }
      literal_list.push_back(cube_lits);
    }
    auto cover = SopCover(var_num(), literal_list);
    return cover;
  }

  /// @brief 変数の数を得る．
  SizeType
  var_num() const
  {
    return mAssignList.size();
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

  /// @brief リテラルに変換する．
  Literal
  to_literal(
    const Assign& assign ///< [in] 値の割当
  ) const
  {
    auto key = make_key(assign);
    if ( mMap.count(key) == 0 ) {
      throw std::invalid_argument{"assign is not registered"};
    }
    auto id = mMap.at(key);
    bool inv = assign.val() == false;
    return Literal{id, inv};
  }

  /// @brief 変数番号から元の割当を得る．
  Assign
  to_assign(
    SizeType id
  ) const
  {
    if ( id >= mAssignList.size() ) {
      throw std::out_of_range{"id is out of range"};
    }
    return mAssignList[id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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
  vector<Assign> mAssignList;

};

END_NAMESPACE_DRUID

#endif // ASSIGNVARDICT_H
