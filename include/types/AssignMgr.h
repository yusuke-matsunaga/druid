#ifndef ASSIGNMGR_H
#define ASSIGNMGR_H

/// @file AssignMgr.h
/// @brief AssignMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AssignMgr AssignMgr.h "AssignMgr.h"
/// @brief Assign と変数番号の対応付けを行うクラス
//////////////////////////////////////////////////////////////////////
class AssignMgr
{
public:

  /// @brief コンストラクタ
  AssignMgr() = default;

  /// @brief デストラクタ
  ~AssignMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief Assign を変数番号に変換する．
  SizeType
  get_varid(
    const Assign& assign
  )
  {
    auto key = assign.val() ? assign : ~assign;
    if ( mVarDict.count(key) > 0 ) {
      return mVarDict.at(key);
    }
    auto varid = mAssignList.size();
    mAssignList.push_back(key);
    mVarDict.emplace(key, varid);
    return varid;
  }

  /// @brief Assign のリストを返す．
  const vector<Assign>&
  assign_list() const
  {
    return mAssignList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Assign をキーにして変数番号を格納する辞書
  std::unordered_map<Assign, SizeType> mVarDict;

  // Assign のリスト
  vector<Assign> mAssignList;

};

END_NAMESPACE_DRUID

#endif // ASSIGNMGR_H
