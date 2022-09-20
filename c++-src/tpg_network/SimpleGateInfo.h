#ifndef SIMPLEGATEINFO_H
#define SIMPLEGATEINFO_H

/// @file SimpleGateInfo.h
/// @brief SimpleGateInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
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
  SimpleGateInfo(
    GateType gate_type ///< [in] ゲートタイプ
  );

  /// @brief デストラクタ
  ~SimpleGateInfo();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込みタイプのときに true を返す．
  bool
  is_simple() const override;

  /// @brief ゲートタイプを返す．
  GateType
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
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 値
  ) const override;


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
