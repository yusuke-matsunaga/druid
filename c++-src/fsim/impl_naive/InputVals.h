#ifndef INPUTVALS_H
#define INPUTVALS_H

/// @file InputVals.h
/// @brief InputVals のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class InputVals InputVals.h "InputVals.h"
/// @brief シミュレーションの入力パタンを表すクラス
///
/// 実際には TestVector と NodeTimeValList の２通りがあるので仮想関数で
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
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const = 0;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  virtual
  void
  set_val1(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const = 0;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  virtual
  void
  set_val2(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const = 0;

  /// @brief 有効なビットを表すビットマスク
  virtual
  PackedVal
  bitmask() const = 0;

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
  explicit
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
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  void
  set_val1(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  void
  set_val2(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 有効なビットを表すビットマスク
  PackedVal
  bitmask() const override;


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
  explicit
  Tv2InputVals(
    const vector<TestVector>& tv_list ///< [in] パタンのリスト
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
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  void
  set_val1(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  void
  set_val2(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 有効なビットを表すビットマスク
  PackedVal
  bitmask() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // パタン数
  SizeType mPatNum;

  // テストベクタの配列
  TestVector mPatArray[PV_BITLEN];

};


//////////////////////////////////////////////////////////////////////
/// @class NvlInputVals InputVals.h "InputVals.h"
/// @brief NodeTimeValList を用いた InputVals の実装
//////////////////////////////////////////////////////////////////////
class NvlInputVals :
  public InputVals
{
public:

  /// @brief コンストラクタ
  explicit
  NvlInputVals(
    const NodeTimeValList& assign_list ///< [in] 値の割り当てリスト
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
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 1時刻目の値を設定する．(遷移故障用)
  void
  set_val1(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 2時刻目の値を設定する．(遷移故障用)
  void
  set_val2(
    FSIM_CLASSNAME& fsim ///< [in] 故障シミュレータ
  ) const override;

  /// @brief 有効なビットを表すビットマスク
  PackedVal
  bitmask() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 値の割り当てリスト
  const NodeTimeValList& mAssignList;

};

END_NAMESPACE_DRUID_FSIM

#endif // INPUTVALS_H
