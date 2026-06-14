#ifndef EQDOMCAND_H
#define EQDOMCAND_H

/// @file EqDomCand.h
/// @brief EqDomCand のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFault.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class EqDomCand EqDomCand.h "EqDomCand.h"
/// @brief 等価故障と支配故障の候補を表すクラス
///
/// - 全ての故障はいずれかの等価故障グループに属する．
/// - 等価故障グループ間には支配関係がある．
//////////////////////////////////////////////////////////////////////
class EqDomCand
{
  using DomPairList = std::vector<std::pair<SizeType, SizeType>>;

public:

  /// @brief コンストラクタ
  EqDomCand() = default;

  /// @brief 内容を指定したコンストラクタ
  EqDomCand(
    const std::vector<TpgFaultList>& group_list, ///< [in] 等価故障グループのリスト
    const DomPairList& dom_list,                 ///< [in] 支配関係のペアのリスト
    bool prune = false                           ///< [in] 推移的な関係を除外する時 true
  );

  /// @brief デストラクタ
  ~EqDomCand() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価故障のグループ数を返す．
  SizeType
  group_num() const
  {
    return mGroupList.size();
  }

  /// @brief 等価故障のグループを返す．
  const TpgFaultList&
  group(
    SizeType id ///< [in] グループ番号 ( 0 <= id < eqgroup_num() )
  ) const
  {
    _check_id(id);
    return mGroupList[id];
  }

  /// @brief 等価故障グループ番号を返す．
  SizeType
  group_id(
    const TpgFault& fault ///< [in] 故障
  ) const
  {
    return mIdMap.at(fault.id());
  }

  /// @brief 支配関係の候補リストを返す．
  const std::vector<SizeType>&
  dom_list(
    SizeType id ///< [in] グループ番号 ( 0 <= id < eqgroup_num() )
  ) const
  {
    _check_id(id);
    return mDomListArray[id];
  }

  /// @brief 支配関係の総数を返す．
  SizeType
  total_num() const;

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;

  /// @brief 等価比較演算子
  bool
  operator==(
    const EqDomCand& right
  ) const
  {
    return mGroupList == right.mGroupList &&
    mDomListArray == right.mDomListArray;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const EqDomCand& right
  ) const
  {
    return !operator==(right);
  }

  void
  check(
    const EqDomCand& right
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief グループ番号をチェックする．
  void
  _check_id(
    SizeType id ///< [in] グループ番号 ( 0 <= id < eqgroup_num() )
  ) const
  {
    if ( id >= group_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 等価故障グループのリスト
  std::vector<TpgFaultList> mGroupList;

  // 故障番号をキーにして等価故障グループ番号を保持する辞書
  std::unordered_map<SizeType, SizeType> mIdMap;

  // 支配関係の候補リストの配列
  // キーはグループ番号
  std::vector<std::vector<SizeType>> mDomListArray;

};

END_NAMESPACE_DRUID

#endif // EQDOMCAND_H
