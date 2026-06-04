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
//////////////////////////////////////////////////////////////////////
class EqDomCand
{
public:

  /// @brief コンストラクタ
  EqDomCand() = default;

  /// @brief デストラクタ
  ~EqDomCand() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の故障のリストを返す．
  const TpgFaultList&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 等価故障のグループ数を返す．
  SizeType
  eqgroup_num() const
  {
    return mEqGroupList.size();
  }

  /// @brief 等価故障のグループを返す．
  const TpgFaultList&
  eqgroup(
    SizeType id ///< [in] グループ番号 ( 0 <= id < eqgroup_num() )
  ) const
  {
    if ( id >= eqgroup_num() ) {
      throw std::out_of_range{"id is ouf of range"};
    }
    return mEqGroupList[id];
  }

  /// @brief 支配故障の候補リストを返す．
  const TpgFaultList&
  domcand(
    const TpgFault& fault ///< [in] 故障
  ) const
  {
    return mDomCandArray[fault.id()];
  }

  /// @brief 支配故障の候補数を返す．
  SizeType
  total_cand_num() const;

  /// @brief 直接支配故障の候補数を返す．
  SizeType
  total_imm_cand_num() const;

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
    return mEqGroupList == right.mEqGroupList &&
    mDomCandArray == right.mDomCandArray;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const EqDomCand& right
  ) const
  {
    return !operator==(right);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障リストを設定する．
  void
  init(
    const TpgFaultList& fault_list ///< [in] 故障リスト
  );

  /// @brief 等価故障グループを追加する．
  void
  add_eqgroup(
    const TpgFaultList& fault_list ///< [in] 等価故障グループのリスト
  )
  {
    mEqGroupList.push_back(fault_list);
  }

  /// @brief 等価故障グループのリストをソートする．
  void
  sort()
  {
    std::sort(mEqGroupList.begin(), mEqGroupList.end(),
	      [](const TpgFaultList& a, const TpgFaultList& b)->bool {
		return a[0].id() < b[0].id();
	      });
  }

  /// @brief 支配故障の候補リストを設定する．
  void
  set_domcand(
    const TpgFault& fault,         ///< [in] 対象の故障
    const TpgFaultList& fault_list ///< [in] 支配故障の候補リスト
  )
  {
    if ( fault.id() >= mDomCandArray.size() ) {
      throw std::out_of_range{"fault.id() is out of range"};
    }
    mDomCandArray[fault.id()] = fault_list;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障リスト
  TpgFaultList mFaultList;

  // 等価故障グループのリスト
  std::vector<TpgFaultList> mEqGroupList;

  // 支配故障の候補リストの配列
  // キーは故障番号
  std::vector<TpgFaultList> mDomCandArray;

};

END_NAMESPACE_DRUID

#endif // EQDOMCAND_H
