#ifndef TESTCOVER_H
#define TESTCOVER_H

/// @file TestCover.h
/// @brief TestCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "AssignList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TestCover TestCover.h "TestCover.h"
/// @brief (拡張)テストカバーを表すクラス
///
/// 各々のキューブに共通なキューブをくくりだしている．
//////////////////////////////////////////////////////////////////////
class TestCover
{
public:

  /// @brief 空のコンストラクタ
  TestCover() = default;

  /// @brief コンストラクタ
  TestCover(
    const TpgFault* fault,              ///< [in] 対象の故障
    const AssignList& common_cube,      ///< [in] 共通の割り当て
    const vector<AssignList>& cube_list ///< [in] 値割り当てのリスト
  ) : mFault{fault},
      mCommonCube{common_cube},
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

  /// @brief 共通な割り当てを返す．
  const AssignList&
  common_cube() const
  {
    return mCommonCube;
  }

  /// @brief 値割り当てのリストのリストを返す．
  const vector<AssignList>&
  cube_list() const
  {
    return mCubeList;
  }

  /// @brief キューブ数を返す．
  SizeType
  cube_num() const
  {
    return mCubeList.size();
  }

  /// @brief リテラル数を返す．
  SizeType
  literal_num() const
  {
    SizeType nl = mCommonCube.size();
    for ( auto& cube: mCubeList ) {
      nl += cube.size();
    }
    return nl;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障
  const TpgFault* mFault{nullptr};

  // 共通な割り当て
  AssignList mCommonCube;

  // 値割り当てのリスト
  vector<AssignList> mCubeList;

};

END_NAMESPACE_DRUID

#endif // TESTCOVER_H
