#ifndef ASSIGNEXPR_H
#define ASSIGNEXPR_H

/// @file AssignExpr.h
/// @brief AssignExpr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Assign.h"


BEGIN_NAMESPACE_DRUID

class NtvNode;

//////////////////////////////////////////////////////////////////////
/// @class AssignExpr AssignExpr.h "AssignExpr.h"
/// @brief Assign を用いた論理式を表すクラス
///
/// 以下のタイプを持つ．
/// - リテラル: Assign に対応する．
/// - AND:     AssignExpr の論理積を表す．
/// - OR:      AssignExpr の論理和を表す．
//////////////////////////////////////////////////////////////////////
class AssignExpr
{
public:

  /// @brief リテラルを作る．
  static
  AssignExpr
  make_literal(
    const Assign& ntv ///< [in] 割り当て
  )
  {
    return AssignExpr{ntv};
  }

  /// @brief 論理積を作る．
  static
  AssignExpr
  make_and(
    const vector<AssignExpr>& opr_list
  );

  /// @brief 論理積を作る．
  static
  AssignExpr
  make_and(
    const AssignList& opr_list
  );

  /// @brief 論理和を作る．
  static
  AssignExpr
  make_or(
    const vector<AssignExpr>& opr_list
  );

  /// @brief 空のコンストラクタ
  AssignExpr() = default;

  /// @brief リテラル用のコンストラクタ
  AssignExpr(
    const Assign& literal
  ) : mType{1},
      mLiteral{literal}
  {
  }

  /// @brief AND/OR用のコンストラクタ
  AssignExpr(
    int type,
    const vector<AssignExpr>& opr_list
  ) : mType{static_cast<std::uint8_t>(type)},
      mOprList{opr_list}
  {
  }

  /// @brief デストラクタ
  ~AssignExpr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief リテラルの時 true を返す．
  bool
  is_literal() const
  {
    return mType == 1;
  }

  /// @brief リテラルの時 値割り当てを返す．
  Assign
  literal() const
  {
    return mLiteral;
  }

  /// @brief 論理積の時 true を返す．
  bool
  is_and() const
  {
    return mType == 2;
  }

  /// @brief 論理和の時 true を返す．
  bool
  is_or() const
  {
    return mType == 3;
  }

  /// @brief オペランドを返す．
  const vector<AssignExpr>&
  opr_list() const
  {
    return mOprList;
  }

  /// @brief 含まれるキューブを取り出す．
  AssignList
  extract_cube() const;

  /// @brief リテラル数を返す．
  SizeType
  literal_num() const;

  /// @brief AND演算子
  AssignExpr
  operator&(
    const AssignExpr& right
  ) const;

  /// @brief AND演算付き代入
  AssignExpr&
  operator&=(
    const AssignExpr& right
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // タイプ
  std::uint8_t mType;

  // リテラル
  Assign mLiteral;

  // オペランドのリスト
  vector<AssignExpr> mOprList;

};

END_NAMESPACE_DRUID

#endif // ASSIGNEXPR_H
