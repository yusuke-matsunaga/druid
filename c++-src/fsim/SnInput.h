﻿#ifndef SNINPUT_H
#define SNINPUT_H

/// @file SnInput.h
/// @brief SnInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "SimNode.h"


BEGIN_NAMESPACE_SATPG_FSIM

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
  SnInput(int id);

  /// @brief デストラクタ
  virtual
  ~SnInput();


public:

  /// @brief ゲートタイプを返す．
  ///
  /// ここでは kGateBUFF を返す．
  virtual
  GateType
  gate_type() const override;

  /// @brief ファンイン数を得る．
  virtual
  int
  fanin_num() const override;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(int pos) const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val() override;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(int ipos) override;


public:
  //////////////////////////////////////////////////////////////////////
  // SimNode の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をダンプする．
  virtual
  void
  dump(ostream& s) const override;

};

END_NAMESPACE_SATPG_FSIM

#endif // SNINPUT_H
