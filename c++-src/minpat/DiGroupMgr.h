#ifndef DIGROUPMGR_H
#define DIGROUPMGR_H

/// @file DiGroupMgr.h
/// @brief DiGroupMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DiGroup.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DiGroupMgr DiGroupMgr.h "DiGroupMgr.h"
/// @brief DiGroup を管理するクラス
///
/// 以下の情報を持つ．
/// - DiGroup のリスト
/// 各 DiGroup が持つ conflict_list や dominate_list に含まれる
/// DiGroup も同じ DiGroupMgr に属している．
//////////////////////////////////////////////////////////////////////
class DiGroupMgr
{
public:

  /// @brief 初期グループを作るコンストラクタ
  explicit
  DiGroupMgr(
    const TpgFaultList& fault_list ///< [in] 故障リスト
  );

  /// @brief 細分化したグループを作るコンストラクタ
  explicit
  DiGroupMgr(
    const DiGroupMgr& prev_mgr,                   ///< [in] 元となる故障グループのリスト
    const std::unordered_set<SizeType>& fault_set ///< [in] 検出された故障番号を表す集合
  );

  /// @brief ムーブコンストラクタ
  DiGroupMgr(
    DiGroupMgr&& right
  ) = default;

  /// @brief ムーブ代入演算子
  DiGroupMgr&
  operator=(
    DiGroupMgr&& right
  ) = default;

  /// @brief デストラクタ
  ~DiGroupMgr() = default;


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

  /// @brief 故障グループのリストを返す．
  const std::vector<const DiGroup*>&
  group_list() const
  {
    return mGroupList;
  }

  /// @brief 未検出グループを返す．
  ///
  /// nullptr の場合もある．
  const DiGroup*
  undet_group() const
  {
    return mUndetGroup;
  }

  /// @brief 等価比較演算子
  ///
  /// 自身が right の細分化であると仮定して以下の条件をチェックする．
  /// - グループ数が等しい
  /// - undet_group() が共に nullptr であるか共に nullptr でない．
  bool
  operator==(
    const DiGroupMgr& right ///< [in] 比較対象
  ) const
  {
    if ( group_num() != right.group_num() ) {
      return false;
    }
    if ( undet_group() != nullptr ) {
      // right.undet_group() も nullptr ではない．
      return true;
    }
    return right.undet_group() == nullptr;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const DiGroupMgr& right ///< [in] 比較対象
  ) const
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障グループを作る．
  DiGroup*
  new_group(
    const TpgFaultList& fault_list ///< [in] 故障リスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 生成された DiGroup の所有権を持つリスト
  std::vector<std::unique_ptr<DiGroup>> mGroupArray;

  // DiGroup のリスト
  std::vector<const DiGroup*> mGroupList;

  // 一度も検出されていない故障グループ
  DiGroup* mUndetGroup{nullptr};

};

END_NAMESPACE_DRUID

#endif // DIGROUPMGR_H
