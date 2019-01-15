#ifndef SIMPLEGATEINFO_H
#define SIMPLEGATEINFO_H

/// @file SimpleGateInfo.h
/// @brief SimpleGateInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgGateInfo.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class SimpleGateInfo SimpleGateInfo.h "SimpleGateInfo.h"
/// @brief 組み込み型の TpgGateInfo
//////////////////////////////////////////////////////////////////////
class SimpleGateInfo :
  public TpgGateInfo
{
public:

  /// @brief コンストラクタ
  /// @param[in] gate_type ゲートタイプ
  SimpleGateInfo(GateType gate_type);

  /// @brief デストラクタ
  virtual
  ~SimpleGateInfo();


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

  // ゲートタイプ
  GateType mGateType;

  // 制御値
  Val3 mCVal[2];

};

END_NAMESPACE_DRUID

#endif // SIMPLEGATEINFO_H
