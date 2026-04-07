#ifndef DIGROUP_H
#define DIGROUP_H

/// @file DiGroup.h
/// @brief DiGroup のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiGroup DiGroup.h "DiGroup.h"
/// @brief 二分法で用いる故障グループを表すクラス
///
/// 以下の情報を持つ．
/// - ID番号
/// - 故障のリスト
/// - conflict している候補グループのリスト
/// - dominate している候補グループのリスト
///
/// ID番号は親の DiGroupMgr に対して唯一のものとなっている．
//////////////////////////////////////////////////////////////////////
class DiGroup
{
public:

  /// @brief コンストラクタ
  DiGroup(
    SizeType id,                   ///< [in] ID番号
    const TpgFaultList& fault_list ///< [in] 故障のリスト
  ) : mId{id},
      mFaultList{fault_list}
  {
  }

  /// @brief デストラクタ
  ~DiGroup() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 故障のリストを返す．
  const TpgFaultList&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief conflicit している候補グループのリストを返す．
  const std::vector<DiGroup*>&
  conflict_list() const
  {
    return mConflictList;
  }

  /// @brief dominate している候補グループのリストを返す．
  const std::vector<DiGroup*>&
  dominate_list() const
  {
    return mDominateList;
  }

  /// @brief conflict_list を設定する．
  void
  set_conflict_list(
    std::vector<DiGroup*>&& conflict_list ///< [in] 設定するグループのリスト
  )
  {
    std::swap(mConflictList, conflict_list);
  }

  /// @brief dominate_list を設定する．
  void
  set_dominate_list(
    std::vector<DiGroup*>&& dominate_list ///< [in] 設定するグループのリスト
  )
  {
    std::swap(mDominateList, dominate_list);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 故障のリスト
  TpgFaultList mFaultList;

  // conflict している候補のリスト
  std::vector<DiGroup*> mConflictList;

  // dominate している候補のリスト
  std::vector<DiGroup*> mDominateList;

};

END_NAMESPACE_DRUID

#endif // DIGROUP_H
