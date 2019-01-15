#ifndef SNBUFF_H
#define SNBUFF_H

/// @file SnBuff.h
/// @brief SnBuff のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
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
  SnBuff(int id,
	 const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnBuff();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const override;


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
  SnNot(int id,
	const vector<SimNode*>& inputs);

  /// @brief デストラクタ
  virtual
  ~SnNot();


public:

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  virtual
  FSIM_VALTYPE
  _calc_val() override;

};

END_NAMESPACE_DRUID_FSIM

#endif // SNBUFF_H
