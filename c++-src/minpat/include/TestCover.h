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
/// - 実際にはカバーとは別に各々のキューブに共通なキューブを持つ．
/// - 入力は AssignList で与えられるが内部では変数番号を用いる．
/// - そのため Assign と番号の対応付けを保持する辞書を持つ．
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
  );

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

  /// @brief 用いられている変数の数を返す．
  SizeType
  variable_num() const
  {
    return mAssignList.size();
  }

  /// @biref 変数番号に対応する割り当てを返す．
  Assign
  assign(
    SizeType var ///< [in] 変数番号
  ) const
  {
    if ( var >= variable_num() ) {
      throw std::invalid_argument{"var is out of range"};
    }
    return mAssignList[var];
  }

  /// @brief 共通なキューブを返す．
  const vector<SizeType>&
  common_cube() const
  {
    return mCommonCube;
  }

  /// @brief キューブのリストを返す．
  const vector<vector<SizeType>>&
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

  // 割り当てのリスト
  AssignList mAssignList;

  // 共通なキューブ
  vector<SizeType> mCommonCube;

  // 値割り当てのリスト
  vector<vector<SizeType>> mCubeList;

};

END_NAMESPACE_DRUID

#endif // TESTCOVER_H
