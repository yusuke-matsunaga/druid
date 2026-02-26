#ifndef RESULTREP_H
#define RESULTREP_H

/// @file ResultRep.h
/// @brief ResultRep の派生クラスのヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/DtpgResults.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class ResultRep ResultRep.h "ResultRep.h"
/// @brief DtpgResults の内容を表す基底クラス
///
/// - 検出された場合
///   * TestVector
///   * AssignList
/// - 検出不能の場合
/// を表す．
//////////////////////////////////////////////////////////////////////
class ResultRep
{
public:

  /// @brief デストラクタ
  virtual
  ~ResultRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 複製を作る．
  virtual
  ResultRep*
  duplicate() const = 0;

  /// @brief 結果を返す．
  virtual
  FaultStatus
  status() const = 0;

  /// @brief テストベクタを持つ時 true を返す．
  virtual
  bool
  has_testvector() const;

  /// @brief テストベクタを返す．
  ///
  /// has_testvector() == false の場合は std::logic_error 例外を送出する．
  virtual
  const TestVector&
  testvector() const;

  /// @brief 値割り当てを持つ時 true を返す．
  virtual
  bool
  has_assign_list() const;

  /// @brief 値割り当てを返す．
  ///
  /// has_assign_list() == false の場合は std::logic_error 例外を送出する．
  virtual
  const AssignList&
  assign_list() const;

};


//////////////////////////////////////////////////////////////////////
/// @class ResultRep_TV ResultRep_TV.h "ResultRep_TV.h"
/// @brief TestVector を持つ ResultRep
//////////////////////////////////////////////////////////////////////
class ResultRep_TV:
  public ResultRep
{
public:

  /// @brief コンストラクタ
  ResultRep_TV(
    const TestVector& testvector
  ) : mTestVector{testvector}
  {
  }

  /// @brief デストラクタ
  ~ResultRep_TV() = default;

  /// @brief 複製を作る．
  ResultRep*
  duplicate() const override;

  /// @brief 結果を返す．
  FaultStatus
  status() const override;

  /// @brief テストベクタを持つ時 true を返す．
  bool
  has_testvector() const override;

  /// @brief テストベクタを返す．
  const TestVector&
  testvector() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // テストベクタ
  TestVector mTestVector;

};


//////////////////////////////////////////////////////////////////////
/// @class ResultRep_AL ResultRep_AL.h "ResultRep_AL.h"
/// @brief AssignList を持つ ResultRep
//////////////////////////////////////////////////////////////////////
class ResultRep_AL:
  public ResultRep
{
public:

  // コンストラクタ
  ResultRep_AL(
    const AssignList& assign_list
  ) : mAssignList{assign_list}
  {
  }

  // デストラクタ
  ~ResultRep_AL() = default;

  /// @brief 複製を作る．
  ResultRep*
  duplicate() const override;

  // 結果を返す．
  FaultStatus
  status() const override;

  // 値割り当てを持つ時 true を返す．
  bool
  has_assign_list() const override;

  // 値割り当てを返す．
  const AssignList&
  assign_list() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 値割り当て
  AssignList mAssignList;

};


//////////////////////////////////////////////////////////////////////
/// @class ResultRep_UT ResultRep_UT.h "ResultRep_UT.h"
/// @brief 検出不能を表す ResultRep
//////////////////////////////////////////////////////////////////////
class ResultRep_UT:
  public ResultRep
{
public:

  // コンストラクタ
  ResultRep_UT() = default;

  // デストラクタ
  ~ResultRep_UT() = default;

  /// @brief 複製を作る．
  ResultRep*
  duplicate() const override;

  // 結果を返す．
  FaultStatus
  status() const override;

};

END_NAMESPACE_DRUID

#endif // RESULTREP_H
