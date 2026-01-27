#ifndef SNOR_H
#define SNOR_H

/// @file SnOr.h
/// @brief SnOr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SnGate.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnOr SimNode.h
/// @brief ORノード
//////////////////////////////////////////////////////////////////////
class SnOr :
  public SnGate
{
public:

  /// @brief コンストラクタ
  SnOr(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnOr() = default;


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

  /// @brief ファンインの値のORを計算する．
  FSIM_VALTYPE
  _calc_or(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val = val_array[_fanin(0)];
    for ( auto i: Range(1, _fanin_num()) ) {
      val |= val_array[_fanin(i)];
    }
    return val;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnOr2 SimNode.h
/// @brief 2入力ORノード
//////////////////////////////////////////////////////////////////////
class SnOr2 :
  public SnGate2
{
public:

  /// @brief コンストラクタ
  SnOr2(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate2{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnOr2() = default;


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

  /// @brief ファンインの値のORを計算する．
  FSIM_VALTYPE
  _calc_or(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val0 = val_array[_fanin(0)];
    auto val1 = val_array[_fanin(1)];
    return val0 | val1;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnOr3 SimNode.h
/// @brief 3入力ORノード
//////////////////////////////////////////////////////////////////////
class SnOr3 :
  public SnGate3
{
public:

  /// @brief コンストラクタ
  SnOr3(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate3{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnOr3() = default;


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

  /// @brief ファンインの値のORを計算する．
  FSIM_VALTYPE
  _calc_or(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val0 = val_array[_fanin(0)];
    auto val1 = val_array[_fanin(1)];
    auto val2 = val_array[_fanin(2)];
    return val0 | val1 | val2;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnOr4 SimNode.h
/// @brief 4入力ORノード
//////////////////////////////////////////////////////////////////////
class SnOr4 :
  public SnGate4
{
public:

  /// @brief コンストラクタ
  SnOr4(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnGate4{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnOr4() = default;


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

  /// @brief ファンインの値のORを計算する．
  FSIM_VALTYPE
  _calc_or(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    auto val0 = val_array[_fanin(0)];
    auto val1 = val_array[_fanin(1)];
    auto val2 = val_array[_fanin(2)];
    auto val3 = val_array[_fanin(3)];
    return val0 | val1 | val2 | val3;
  }

};


//////////////////////////////////////////////////////////////////////
/// @class SnNor SimNode.h
/// @brief NORノード
//////////////////////////////////////////////////////////////////////
class SnNor :
  public SnOr
{
public:

  /// @brief コンストラクタ
  SnNor(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnOr{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNor() = default;


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
/// @class SnNor2 SimNode.h
/// @brief 2入力NORノード
//////////////////////////////////////////////////////////////////////
class SnNor2 :
  public SnOr2
{
public:

  /// @brief コンストラクタ
  SnNor2(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnOr2{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNor2() = default;


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
/// @class SnNor3 SimNode.h
/// @brief 3入力NORノード
//////////////////////////////////////////////////////////////////////
class SnNor3 :
  public SnOr3
{
public:

  /// @brief コンストラクタ
  SnNor3(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnOr3{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNor3() = default;


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
/// @class SnNor2 SimNode.h
/// @brief 2入力NORノード
//////////////////////////////////////////////////////////////////////
class SnNor4 :
  public SnOr4
{
public:

  /// @brief コンストラクタ
  SnNor4(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  ) : SnOr4{id, level, inputs}
  {
  }

  /// @brief デストラクタ
  ~SnNor4() = default;


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

#endif // SNOR_H
