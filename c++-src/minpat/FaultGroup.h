#ifndef FAULTGROUP_H
#define FAULTGROUP_H

/// @file FaultGroup.h
/// @brief FaultGroup のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultGroup FaultGroup.h "FaultGroup.h"
/// @brief パタン圧縮用の故障グループを表すクラス
///
/// - 故障のリストと現在のテストベクタ(テストキューブ)を持つ．
/// - コンストラクタで与えられた内容を保持するだけなので
///   Python の named tuple のようなクラス
//////////////////////////////////////////////////////////////////////
class FaultGroup
{
public:

  /// @brief 空のコンストラクタ
  FaultGroup() = default;

  /// @brief 内容を指定したコンストラクタ
  FaultGroup(
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    const TestVector& testvector    ///< [in] テストベクタ
  ) : mFaultList{fault_list},
      mTestVector{testvector}
  {
  }

  /// @brief デストラクタ
  ~FaultGroup() = default;

  /// @brief テストベクタのリストから FaultGroup のリストを作る．
  static
  std::vector<FaultGroup>
  make_list(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
    const TpgFaultList& fault_list          ///< [in] 対象の故障リスト
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のリストを得る．
  const TpgFaultList&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 現在のテストベクタを得る．
  const TestVector&
  testvector() const
  {
    return mTestVector;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障のリスト
  TpgFaultList mFaultList;

  // 現在のテストベクタ
  TestVector mTestVector;

};

END_NAMESPACE_DRUID

#endif // FAULTGROUP_H
