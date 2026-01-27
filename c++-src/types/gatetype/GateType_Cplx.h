#ifndef GATETYPE_CPLX_H
#define GATETYPE_CPLX_H

/// @file GateType.h
/// @brief GateType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateType_Cplx GateType_Cplx.h "GateType_Cplx.h"
/// @brief 複合型の GateType
//////////////////////////////////////////////////////////////////////
class GateType_Cplx :
  public GateType
{
public:

  /// @brief コンストラクタ
  GateType_Cplx(
    SizeType id,     ///< [in] ID番号
    SizeType ni,     ///< [in] 入力数
    const Expr& expr ///< [in] 論理式
  );

  /// @brief デストラクタ
  ~GateType_Cplx() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief 論理式タイプのときに true を返す．
  bool
  is_expr() const override;

  /// @brief 論理式を返す．
  Expr
  expr() const override;

  /// @brief 追加ノード数を返す．
  SizeType
  extra_node_num() const override;

  /// @brief 制御値を返す．
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式
  Expr mExpr;

  // 追加のノード数
  SizeType mExtraNodeNum;

  // 制御値の配列
  std::vector<Val3> mCVal;

};

END_NAMESPACE_DRUID

#endif // GATETYPE_CPLX_H
