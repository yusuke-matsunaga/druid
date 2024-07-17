#ifndef TESTCUBE_H
#define TESTCUBE_H

/// @file TestCube.h
/// @brief TestCube のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeTimeValList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TestCube TestCube.h "TestCube.h"
/// @brief (拡張)テストキューブを表すクラス
//////////////////////////////////////////////////////////////////////
class TestCube
{
public:

  /// @brief コンストラクタ
  TestCube() = default;

  /// @brief 内容を指定したコンストラクタ
  TestCube(
    const NodeTimeValList& assignments, ///< [in] 値割り当てのリスト
    const TpgFault* fault               ///< [in] 対象の故障
  ) : mAssignments{assignments},
      mFault{fault}
  {
  }

  /// @brief デストラクタ
  ~TestCube() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割り当てのリストを返す．
  const NodeTimeValList&
  assignments() const
  {
    return mAssignments;
  }

  /// @brief 対象の故障を返す．
  const TpgFault*
  fault() const
  {
    return mFault;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 値割り当てのリスト
  NodeTimeValList mAssignments;

  // 対象の故障
  const TpgFault* mFault;

};

END_NAMESPACE_DRUID

#endif // TESTCUBE_H
