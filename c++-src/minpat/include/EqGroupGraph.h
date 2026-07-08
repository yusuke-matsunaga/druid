#ifndef EQGROUPGRAPH_H
#define EQGROUPGRAPH_H

/// @file EqGroupGraph.h
/// @brief EqGroupGraph のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class EqGroupMgr;

//////////////////////////////////////////////////////////////////////
/// @class EqGroupGraph EqGroupGraph.h "EqGroupGraph.h"
/// @brief EqGroupMgr の現在の状態を表すクラス
//////////////////////////////////////////////////////////////////////
class EqGroupGraph
{
public:

  /// @brief 空のコンストラクタ
  EqGroupGraph() = default;

  /// @brief EqGroupMgr の現在の内容を取り出すコンストラクタ
  EqGroupGraph(
    const EqGroupMgr& mgr
  );

  /// @brief デストラクタ
  ~EqGroupGraph() = default;


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
    const EqGroupGraph& right
  ) const;

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const EqGroupGraph& right
  ) const
  {
    return !operator==(right);
  }

  /// @brief 詳細な比較を行う．
  static
  void
  print_diff(
    std::ostream& s,
    const EqGroupGraph& left,
    const EqGroupGraph& right
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

#endif // EQGROUPGRAPH_H
