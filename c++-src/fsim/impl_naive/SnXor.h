#ifndef SNXOR_H
#define SNXOR_H

/// @file SnXor.h
/// @brief SnXor のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnGate.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnXor SimNode.h
/// @brief XORノード
//////////////////////////////////////////////////////////////////////
class SnXor :
  public SnGate
{
public:

  /// @brief コンストラクタ
  SnXor(
    SizeType id,
    const std::vector<SimNode*>& inputs
  ) : SnGate{id, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnXor() = default;


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
  _calc_val() override;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  PackedVal
  _calc_gobs(
    SizeType ipos
  ) override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインの値のXORを計算する．
  FSIM_VALTYPE
  _calc_xor()
  {
    auto val = _fanin(0)->val();
    for ( auto i: Range(1, _fanin_num()) ) {
      val ^= _fanin(i)->val();
    }
    return val;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnNor2 SimNode.h
/// @brief 2入力XORノード
//////////////////////////////////////////////////////////////////////
class SnXor2 :
  public SnGate2
{
public:

  /// @brief コンストラクタ
  SnXor2(
    SizeType id,
    const std::vector<SimNode*>& inputs
  ) : SnGate2{id, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnXor2() = default;


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
  _calc_val() override;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  PackedVal
  _calc_gobs(
    SizeType ipos
  ) override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインの値のXORを計算する．
  FSIM_VALTYPE
  _calc_xor()
  {
    auto val0 = _fanin(0)->val();
    auto val1 = _fanin(1)->val();
    return val0 ^ val1;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnXnor SimNode.h
/// @brief XNORノード
//////////////////////////////////////////////////////////////////////
class SnXnor :
  public SnXor
{
public:

  /// @brief コンストラクタ
  SnXnor(
    SizeType id,
    const std::vector<SimNode*>& inputs
  ) : SnXor{id, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnXnor() = default;


public:

  /// @brief ゲートタイプを返す．
  PrimType
  gate_type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 2値版の故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  FSIM_VALTYPE
  _calc_val() override;

};


//////////////////////////////////////////////////////////////////////
/// @class SnNor2 SimNode.h
/// @brief 2入力XNORノード
//////////////////////////////////////////////////////////////////////
class SnXnor2 :
  public SnXor2
{
public:

  /// @brief コンストラクタ
  SnXnor2(
    SizeType id,
    const std::vector<SimNode*>& inputs
  ) : SnXor2{id, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnXnor2() = default;


public:

  /// @brief ゲートタイプを返す．
  PrimType
  gate_type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．(2値版)
  FSIM_VALTYPE
  _calc_val() override;

};

END_NAMESPACE_DRUID_FSIM

#endif // SNXOR_H
