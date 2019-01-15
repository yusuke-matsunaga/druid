#ifndef CPLXGATEINFO_H
#define CPLXGATEINFO_H

/// @file CplxGateInfo.h
/// @brief CplxGateInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
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
  /// @param[in] ni 入力数
  /// @param[in] expr 論理式
  CplxGateInfo(int ni,
	       const Expr& expr);

  /// @brief デストラクタ
  virtual
  ~CplxGateInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込みタイプのときに true を返す．
  virtual
  bool
  is_simple() const override;

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const override;

  /// @brief 論理式を返す．
  virtual
  Expr
  expr() const override;

  /// @brief 追加ノード数を返す．
  virtual
  int
  extra_node_num() const override;

  /// @brief 制御値を返す．
  /// @param[in] pos 入力位置
  /// @param[in] val 値
  virtual
  Val3
  cval(int pos,
       Val3 val) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 論理式
  Expr mExpr;

  // 追加のノード数
  int mExtraNodeNum;

  // 制御値の配列
  vector<Val3> mCVal;

};

END_NAMESPACE_DRUID

#endif // CPLXGATEINFO_H
