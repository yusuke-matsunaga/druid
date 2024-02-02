﻿#ifndef SNINPUT_H
#define SNINPUT_H

/// @file SnInput.h
/// @brief SnInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnInput SimNode.h
/// @brief 入力ノード
//////////////////////////////////////////////////////////////////////
class SnInput :
  public SimNode
{
public:

  /// @brief コンストラクタ
  explicit
  SnInput(
    SizeType id
  );

  /// @brief デストラクタ
  ~SnInput();


public:

  /// @brief ゲートタイプを返す．
  ///
  /// ここでは kGateBUFF を返す．
  PrimType
  gate_type() const override;

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const override;

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  fanin(
    SizeType pos
  ) const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  FSIM_VALTYPE
  _calc_val(
    const vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) override;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  PackedVal
  _calc_gobs(
    const vector<FSIM_VALTYPE>& val_array, ///< [in] 値の配列
    SizeType ipos
  ) override;


public:
  //////////////////////////////////////////////////////////////////////
  // SimNode の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をダンプする．
  void
  dump(
    ostream& s
  ) const override;

};

END_NAMESPACE_DRUID_FSIM

#endif // SNINPUT_H
