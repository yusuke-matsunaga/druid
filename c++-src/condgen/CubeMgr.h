#ifndef CUBEMGR_H
#define CUBEMGR_H

/// @file CubeMgr.h
/// @brief CubeMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "AssignList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CubeMgr CubeMgr.h "CubeMgr.h"
/// @brief キューブ(AssignList)を管理するクラス
//////////////////////////////////////////////////////////////////////
class CubeMgr
{
public:

  /// @brief コンストラクタ
  CubeMgr() = default;

  /// @brief デストラクタ
  ~CubeMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 登録されているキューブ数を返す．
  SizeType
  cube_num() const
  {
    return mCubeList.size();
  }

  /// @brief キューブを登録する．
  /// @return キューブ番号を返す．
  SizeType
  reg_cube(
    const AssignList& cube
  )
  {
    auto sig = gen_signature(cube);
    if ( mCubeDict.count(sig) == 0 ) {
      auto id = mCubeList.size();
      mCubeList.push_back(cube);
      mCubeDict.emplace(sig, id);
      return id;
    }
    return mCubeDict.at(sig);
  }

  /// @brief キューブを取り出す．
  const AssignList&
  get_cube(
    SizeType id ///< [in] キューブ番号
  ) const
  {
    if ( id >= cube_num() ) {
      throw std::invalid_argument{"id is out of range"};
    }
    return mCubeList[id];
  }

  /// @brief 登録されているキューブのリストを返す．
  const vector<AssignList>&
  cube_list() const
  {
    return mCubeList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューブのシグネチャを作る．
  static
  string
  gen_signature(
    const AssignList& cube
  )
  {
    ostringstream buf;
    const char* sep = "";
    for ( auto as: cube ) {
      buf << sep;
      sep = " ";
      buf << as.hash();
    }
    return buf.str();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // キューブのリスト
  vector<AssignList> mCubeList;

  // キューブのシグネチャをキーにして mCubeList 中の位置を持つ辞書
  std::unordered_map<string, SizeType> mCubeDict;

};

END_NAMESPACE_DRUID

#endif // CUBEMGR_H
