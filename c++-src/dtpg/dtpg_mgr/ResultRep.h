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
// クラス ResultRep_TV
//////////////////////////////////////////////////////////////////////
class ResultRep_TV:
  public DtpgResults::ResultRep
{
public:

  // コンストラクタ
  ResultRep_TV(
    const TestVector& testvector
  ) : mTestVector{testvector}
  {
  }

  // デストラクタ
  ~ResultRep_TV() = default;

  // 結果を返す．
  FaultStatus
  status() const override;

  // テストベクタを持つ時 true を返す．
  bool
  has_testvector() const override;

  // テストベクタを返す．
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
// クラス ResultRep_AL
//////////////////////////////////////////////////////////////////////
class ResultRep_AL:
  public DtpgResults::ResultRep
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
// クラス ResultRep_UT
//////////////////////////////////////////////////////////////////////
class ResultRep_UT:
  public DtpgResults::ResultRep
{
public:

  // コンストラクタ
  ResultRep_UT() = default;

  // デストラクタ
  ~ResultRep_UT() = default;

  // 結果を返す．
  FaultStatus
  status() const override;

};

END_NAMESPACE_DRUID

#endif // RESULTREP_H
