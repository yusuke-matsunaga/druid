#ifndef VARMGR_H
#define VARMGR_H

/// @file VarMgr.h
/// @brief VarMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Assign.h"
#include "ym/Literal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class VarMgr VarMgr.h "VarMgr.h"
/// @brief Assign のノードと変数の対応を取るためのクラス
//////////////////////////////////////////////////////////////////////
class VarMgr
{
public:

  /// @brief コンストラクタ
  VarMgr() = default;

  /// @brief デストラクタ
  ~VarMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

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
  reg_var(
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
  )
  {
    auto id = reg_var(assign);
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

#endif // VARMGR_H
