#ifndef CPLXGATEINFO_H
#define CPLXGATEINFO_H

/// @file CplxGateInfo.h
/// @brief CplxGateInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgGateInfo.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CplxGateInfo CplxGateInfo.h "CplxGateInfo.h"
/// @brief 複合型の TpgGateInfo
//////////////////////////////////////////////////////////////////////
class CplxGateInfo :
  public TpgGateInfo
{
public:

  /// @brief コンストラクタ
  CplxGateInfo(
    SizeType ni,     ///< [in] 入力数
    const Expr& expr ///< [in] 論理式
  );

  /// @brief デストラクタ
  ~CplxGateInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込みタイプのときに true を返す．
  bool
  is_simple() const override;

  /// @brief ゲートタイプを返す．
  PrimType
  gate_type() const override;

  /// @brief 論理式を返す．
  Expr
  expr() const override;

  /// @brief 追加ノード数を返す．
  SizeType
  extra_node_num() const override;

  /// @brief 制御値を返す．
  Val3
  cval(
    SizeType pos,   ///< [in] 入力位置
    Val3 val        ///< [in] 値
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
  vector<Val3> mCVal;

};

END_NAMESPACE_DRUID

#endif // CPLXGATEINFO_H
