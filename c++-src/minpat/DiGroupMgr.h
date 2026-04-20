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
/// 各 DiGroup が持つ conflict_list や dominance_list に含まれる
/// DiGroup も同じ DiGroupMgr に属している．
//////////////////////////////////////////////////////////////////////
class DiGroupMgr
{
  friend class DiGroupBuilder;

public:

  /// @brief 初期グループを作るコンストラクタ
  explicit
  DiGroupMgr(
    const TpgFaultList& fault_list ///< [in] 故障リスト
  );

  /// @brief コピーコンストラクタ
  DiGroupMgr(
    const DiGroupMgr& src
  );

  /// @brief ムーブコンストラクタ
  DiGroupMgr(
    DiGroupMgr&& right
  ) = default;

  /// @brief コピー代入演算子
  DiGroupMgr&
  operator=(
    const DiGroupMgr& src
  );

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

  /// @brief prev_mgr の故障グループを det_list に基づいて細分化する．
  static
  DiGroupMgr
  dichotomy(
    const DiGroupMgr& mgr,       ///< [in] 元となる故障グループのリスト
    const TpgFaultList& det_list ///< [in] 検出された故障のリスト
  );

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

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;

  /// @brief 等価比較演算子
  ///
  /// 自身が right の細分化であると仮定して以下の条件をチェックする．
  /// - グループ数が等しい
  /// - 各グループの dominance リストが等しい
  bool
  operator==(
    const DiGroupMgr& right ///< [in] 比較対象
  ) const;

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

  /// @brief 空のコンストラクタ
  DiGroupMgr() = default;

  /// @brief 複製する．
  void
  _copy(
    const DiGroupMgr& src
  );

  /// @brief 故障グループを作る．
  ///
  /// fault_list が空の場合には nullptr を返す．
  DiGroup*
  _new_group(
    const TpgFaultList& fault_list ///< [in] 故障リスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 生成された DiGroup の所有権を持つリスト
  std::vector<std::unique_ptr<DiGroup>> mGroupArray;

  // DiGroup のリスト
  // 内容は mGroupArray とほぼ同じだが毎回 cast するのがめんどくさいので
  // 無駄を承知でこちらの型も持っておく．
  std::vector<const DiGroup*> mGroupList;

};

END_NAMESPACE_DRUID

#endif // DIGROUPMGR_H
