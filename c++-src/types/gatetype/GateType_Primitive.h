#ifndef GATETYPE_PRIMITIVE_H
#define GATETYPE_PRIMITIVE_H

/// @file GateType.h
/// @brief GateType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateType_Primitive GateType.h "GateType.h"
/// @brief 組み込み型の GateType
//////////////////////////////////////////////////////////////////////
class GateType_Primitive :
  public GateType
{
public:

  /// @brief コンストラクタ
  GateType_Primitive(
    SizeType id,        ///< [in] ID番号
    SizeType input_num, ///< [in] 入力数
    PrimType gate_type  ///< [in] ゲートタイプ
  );

  /// @brief デストラクタ
  ~GateType_Primitive() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief 組み込みタイプのときに true を返す．
  bool
  is_primitive() const override;

  /// @brief ゲートタイプを返す．
  PrimType
  primitive_type() const override;

  /// @brief 制御値を返す．
  Val3
  cval(
    SizeType pos, ///< [in] 入力位置
    Val3 val      ///< [in] 入力値
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  SizeType mInputNum;

  // ゲートタイプ
  PrimType mPrimType;

  // 制御値
  Val3 mCVal[2];

};

END_NAMESPACE_DRUID

#endif // GATETYPE_PRIMITIVE_H
