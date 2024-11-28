#ifndef ASSIGNMAP_H
#define ASSIGNMAP_H

/// @file AssignMap.h
/// @brief AssignMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Assign.h"
#include "ym/Literal.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AssignMap AssignMap.h "AssignMap.h"
/// @brief 変数番号と Assign の対応付けを表すクラス
///
/// 実際にはただの vector<Assign>
//////////////////////////////////////////////////////////////////////
class AssignMap
{
public:

  /// @brief 空のコンストラクタ
  AssignMap() = default;

  /// @brief コンストラクタ
  explicit
  AssignMap(
    const vector<Assign>& assign_list ///< [in] 割り当てのリスト
  ) : mAssignList{assign_list}
  {
  }

  /// @brief デストラクタ
  ~AssignMap() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の数を返す．
  SizeType
  variable_num() const
  {
    return mAssignList.size();
  }

  /// @brief 変数番号に対応する割り当てを返す．
  Assign
  assign(
    SizeType var ///< [in] 変数番号 ( 0 <= var < variable_num() )
  ) const
  {
    if ( var >= variable_num() ) {
      throw std::out_of_range{"var is out of range"};
    }
    return mAssignList[var];
  }

  /// @brief リテラルに対応する割り当てを返す．
  Assign
  assign(
    Literal lit ///< [in] リテラル
  ) const
  {
    auto varid = lit.varid();
    auto as = assign(varid);
    if ( lit.is_negative() ) {
      as = ~as;
    }
    return as;
  }

  /// @brief 割り当てリストを返す．
  const vector<Assign>&
  assign_list() const
  {
    return mAssignList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数番号をキーとして対応する Assign を持つリスト
  vector<Assign> mAssignList;

};

END_NAMESPACE_DRUID

#endif // ASSIGNMAP_H
