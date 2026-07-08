#ifndef EQGROUPSTATE_H
#define EQGROUPSTATE_H

/// @file EqGroupState.h
/// @brief EqGroupState のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class EqGroupMgr;

//////////////////////////////////////////////////////////////////////
/// @class EqGroupState EqGroupState.h "EqGroupState.h"
/// @brief EqGroupMgr の現在の状態を表すクラス
//////////////////////////////////////////////////////////////////////
class EqGroupState
{
public:

  /// @brief 空のコンストラクタ
  EqGroupState() = default;

  /// @brief EqGroupMgr の現在の内容を取り出すコンストラクタ
  EqGroupState(
    const EqGroupMgr& mgr
  );

  /// @brief デストラクタ
  ~EqGroupState() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief グループ数を返す．
  SizeType
  group_num() const
  {
    return mGroupList.size();
  }

  /// @brief グループの故障リストを返す．
  const TpgFaultList&
  fault_list(
    SizeType gid ///< [in] グループ番号 ( 0 <= gid < group_num() )
  ) const
  {
    auto& group = _get_group(gid);
    return group.mFaultList;
  }

  /// @brief 先行グループ番号リストを返す．
  const std::vector<SizeType>&
  pred_list(
    SizeType gid ///< [in] グループ番号 ( 0 <= gid < group_num() )
  ) const
  {
    auto& group = _get_group(gid);
    return group.mPredList;
  }

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;

  /// @brief 等価比較演算子
  bool
  operator==(
    const EqGroupState& right
  ) const;

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const EqGroupState& right
  ) const
  {
    return !operator==(right);
  }

  /// @brief 詳細な比較を行う．
  static
  void
  print_diff(
    std::ostream& s,
    const EqGroupState& left,
    const EqGroupState& right
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障グループの情報
  struct FaultGroup {
    TpgFaultList mFaultList;         ///< 故障リスト
    std::vector<SizeType> mPredList; ///< 先行グループ番号のリスト
  };

  /// @brief グループを取り出す．
  const FaultGroup&
  _get_group(
    SizeType gid ///< [in] グループ番号 ( 0 <= gid < group_num() )
  ) const
  {
    if ( gid >= group_num() ) {
      throw std::out_of_range{"gid is out of range"};
    }
    return mGroupList[gid];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障グループのリスト
  std::vector<FaultGroup> mGroupList;

};

END_NAMESPACE_DRUID

#endif // EQGROUPSTATE_H
