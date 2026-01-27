#ifndef SNBUFF_H
#define SNBUFF_H

/// @file SnBuff.h
/// @brief SnBuff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnGate.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnBuff SimBuff.h
/// @brief BUFFノード
//////////////////////////////////////////////////////////////////////
class SnBuff :
  public SnGate1
{
public:

  /// @brief コンストラクタ
  SnBuff(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate1{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnBuff() = default;


public:

  /// @brief ゲートタイプを返す．
  PrimType
  gate_type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  FSIM_VALTYPE
  _calc_val(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const override;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  PackedVal
  _calc_gobs(
    const std::vector<FSIM_VALTYPE>& val_array, ///< [in] 値の配列
    SizeType ipos
  ) const override;

};


//////////////////////////////////////////////////////////////////////
/// @class SnNot SimNode.h
/// @brief NOTノード
//////////////////////////////////////////////////////////////////////
class SnNot :
  public SnBuff
{
public:

  /// @brief コンストラクタ
  SnNot(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnBuff{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNot() = default;


public:

  /// @brief ゲートタイプを返す．
  PrimType
  gate_type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  FSIM_VALTYPE
  _calc_val(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const override;

};

END_NAMESPACE_DRUID_FSIM

#endif // SNBUFF_H
