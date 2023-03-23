#ifndef GATETYPE_SIMPLE_H
#define GATETYPE_SIMPLE_H

/// @file GateType_Simple.h
/// @brief GateType_Simple のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateType_Simple GateType_Simple.h "GateType_Simple.h"
/// @brief 組み込み型の GateType_Simple
//////////////////////////////////////////////////////////////////////
class GateType_Simple :
  public GateType
{
public:

  /// @brief コンストラクタ
  GateType_Simple(
    PrimType gate_type ///< [in] ゲートタイプ
  );

  /// @brief デストラクタ
  ~GateType_Simple() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 組み込みタイプのときに true を返す．
  bool
  is_simple() const override;

  /// @brief ゲートタイプを返す．
  PrimType
  primitive_type() const override;

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
  PrimType mPrimType;

  // 制御値
  Val3 mCVal[2];

};

END_NAMESPACE_DRUID

#endif // GATETYPE_SIMPLE_H
