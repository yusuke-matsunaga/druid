#ifndef INPUTVALS_H
#define INPUTVALS_H

/// @file InputVals.h
/// @brief InputVals のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "fsim2_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM2

//////////////////////////////////////////////////////////////////////
/// @class InputVals InputVals.h "InputVals.h"
/// @brief シミュレーションの入力パタンを表すクラス
///
/// 実際には TestVector と NodeValList の２通りがあるので仮想関数で
/// 抽象化する．
//////////////////////////////////////////////////////////////////////
class InputVals
{
public:

  /// @brief デストラクタ
  virtual
  ~InputVals() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値を設定する．(縮退故障用)
  virtual
  void
  set_val(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const = 0;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  virtual
  void
  set_val1(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const = 0;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  virtual
  void
  set_val2(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const = 0;

};


//////////////////////////////////////////////////////////////////////
/// @class TvInputVals InputVals.h "InputVals.h"
/// @brief TestVector を用いた InputVals の実装
//////////////////////////////////////////////////////////////////////
class TvInputVals :
  public InputVals
{
public:

  /// @brief コンストラクタ
  TvInputVals(
    const TestVector& testvector ///< [in] テストベクタ
  );

  /// @brief デストラクタ
  ~TvInputVals();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値を設定する．(縮退故障用)
  void
  set_val(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  void
  set_val1(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  void
  set_val2(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタ
  const TestVector& mTestVector;

};


//////////////////////////////////////////////////////////////////////
/// @class Tv2InputVals InputVals.h "InputVals.h"
/// @brief 複数の TestVector を用いた InputVals の実装
//////////////////////////////////////////////////////////////////////
class Tv2InputVals :
  public InputVals
{
public:

  /// @brief コンストラクタ
  Tv2InputVals(
    PackedVal pat_map,     ///< [in] パタンのセットされているビットに1を立てたビットマップ
    TestVector pat_array[] ///< [in] パタンの配列(サイズは PV_BITLEN の固定長)
  );

  /// @brief デストラクタ
  ~Tv2InputVals();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値を設定する．(縮退故障用)
  void
  set_val(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  void
  set_val1(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  void
  set_val2(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // セットされているパタンを表すビットマップ
  PackedVal mPatMap;

  // mPatMap の最初の1のビット位置
  // 全て０の場合には PV_BITLEN が入る．
  SizeType mPatFirstBit;

  // テストベクタの配列
  TestVector mPatArray[PV_BITLEN];

};


//////////////////////////////////////////////////////////////////////
/// @class NvlInputVals InputVals.h "InputVals.h"
/// @brief NodeValList を用いた InputVals の実装
//////////////////////////////////////////////////////////////////////
class NvlInputVals :
  public InputVals
{
public:

  /// @brief コンストラクタ
  NvlInputVals(
    const NodeValList& assign_list ///< [in] 値の割り当てリスト
  );

  /// @brief デストラクタ
  ~NvlInputVals();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値を設定する．(縮退故障用)
  void
  set_val(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  void
  set_val1(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  void
  set_val2(
    FSIM2_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 値の割り当てリスト
  const NodeValList& mAssignList;

};

END_NAMESPACE_DRUID_FSIM2

#endif // INPUTVALS_H
