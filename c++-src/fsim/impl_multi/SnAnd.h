#ifndef SNAND_H
#define SNAND_H

/// @file SnAnd.h
/// @brief SnAnd のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnGate.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnAnd SimNode.h
/// @brief ANDノード
//////////////////////////////////////////////////////////////////////
class SnAnd :
  public SnGate
{
public:

  /// @brief コンストラクタ
  SnAnd(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnAnd() = default;


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


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインの値のANDを計算する．
  FSIM_VALTYPE
  _calc_and(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val = val_array[_fanin(0)];
    for ( auto i: Range(1, _fanin_num()) ) {
      val &= val_array[_fanin(i)];
    }
    return val;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnAnd2 SimNode.h
/// @brief 2入力ANDノード
//////////////////////////////////////////////////////////////////////
class SnAnd2 :
  public SnGate2
{
public:

  /// @brief コンストラクタ
  SnAnd2(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate2{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnAnd2() = default;


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


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインの値のANDを計算する．
  FSIM_VALTYPE
  _calc_and(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val0 = val_array[_fanin(0)];
    auto val1 = val_array[_fanin(1)];
    return val0 & val1;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnAnd3 SimNode.h
/// @brief 3入力ANDノード
//////////////////////////////////////////////////////////////////////
class SnAnd3 :
  public SnGate3
{
public:

  /// @brief コンストラクタ
  SnAnd3(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate3{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnAnd3() = default;


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


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインの値のANDを計算する．
  FSIM_VALTYPE
  _calc_and(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val0 = val_array[_fanin(0)];
    auto val1 = val_array[_fanin(1)];
    auto val2 = val_array[_fanin(2)];
    return val0 & val1 & val2;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnAnd4 SimNode.h
/// @brief 4入力ANDノード
//////////////////////////////////////////////////////////////////////
class SnAnd4 :
  public SnGate4
{
public:

  /// @brief コンストラクタ
  SnAnd4(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate4{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnAnd4() = default;


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


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインの値のANDを計算する．
  FSIM_VALTYPE
  _calc_and(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val0 = val_array[_fanin(0)];
    auto val1 = val_array[_fanin(1)];
    auto val2 = val_array[_fanin(2)];
    auto val3 = val_array[_fanin(3)];
    return val0 & val1 & val2 & val3;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnNand SimNode.h
/// @brief NANDノード
//////////////////////////////////////////////////////////////////////
class SnNand :
  public SnAnd
{
public:

  /// @brief コンストラクタ
  SnNand(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnAnd{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNand() = default;


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


//////////////////////////////////////////////////////////////////////
/// @class SnNand2 SimNode.h
/// @brief 2入力NANDノード
//////////////////////////////////////////////////////////////////////
class SnNand2 :
  public SnAnd2
{
public:

  /// @brief コンストラクタ
  SnNand2(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnAnd2{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNand2() = default;


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


//////////////////////////////////////////////////////////////////////
/// @class SnNand3 SimNode.h
/// @brief 3入力NANDノード
//////////////////////////////////////////////////////////////////////
class SnNand3 :
  public SnAnd3
{
public:

  /// @brief コンストラクタ
  SnNand3(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnAnd3{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNand3() = default;


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


//////////////////////////////////////////////////////////////////////
/// @class SnNand4 SimNode.h
/// @brief 4入力NANDノード
//////////////////////////////////////////////////////////////////////
class SnNand4 :
  public SnAnd4
{
public:

  /// @brief コンストラクタ
  SnNand4(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnAnd4{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNand4() = default;


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
  _calc_val(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const override;

};

END_NAMESPACE_DRUID_FSIM

#endif // SNAND_H
