#ifndef TESTCOVER_H
#define TESTCOVER_H

/// @file TestCover.h
/// @brief TestCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "NodeTimeValList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TestCover TestCover.h "TestCover.h"
/// @brief (拡張)テストキューブを表すクラス
//////////////////////////////////////////////////////////////////////
class TestCover
{
public:

  /// @brief 空のコンストラクタ
  TestCover() = default;

  /// @brief コンストラクタ
  TestCover(
    const TpgFault* fault,                   ///< [in] 対象の故障
    const vector<NodeTimeValList>& cube_list ///< [in] 値割当のリスト
  ) : mFault{fault},
      mCubeList{cube_list}
  {
  }

  /// @brief デストラクタ
  ~TestCover() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の故障を返す．
  const TpgFault*
  fault() const
  {
    return mFault;
  }

  /// @brief 値割り当てのリストのリストを返す．
  const vector<NodeTimeValList>&
  cube_list() const
  {
    return mCubeList;
  }



private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障
  const TpgFault* mFault{nullptr};

  // 値割り当てのリスト
  vector<NodeTimeValList> mCubeList;

};

END_NAMESPACE_DRUID

#endif // TESTCOVER_H
