#ifndef DICHOGROUP_H
#define DICHOGROUP_H

/// @file DichoGroup.h
/// @brief DichoGroup のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DichoGroup DichoGroup.h "DichoGroup.h"
/// @brief 細分化したグループを表すクラス
//////////////////////////////////////////////////////////////////////
class DichoGroup
{
public:

  using Ptr = std::unique_ptr<DichoGroup>;

public:

  /// @brief コンストラクタ
  DichoGroup(
    SizeType id ///< [in] ID番号
  ) : mId{id}
  {
  }

  /// @brief コンストラクタ
  DichoGroup(
    SizeType id,                   ///< [in] ID番号
    const TpgFaultList& fault_list ///< [in] 故障のリスト
  ) : mId{id},
      mFaultList{fault_list}
  {
  }

  /// @brief デストラクタ
  ~DichoGroup() = default;


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

  /// @brief 故障を追加する．
  void
  add_fault(
    const TpgFault& fault ///< [in] 追加する故障
  )
  {
    mFaultList.push_back(fault);
  }

  /// @brief 後続グループのリストを返す．
  const std::vector<DichoGroup*>&
  succ_list() const
  {
    return mSuccList;
  }

  /// @brief succ_list を設定する．
  void
  set_succ_list(
    std::vector<DichoGroup*>&& src_list ///< [in] 設定するグループのリストの右辺値
  )
  {
    std::swap(mSuccList, src_list);
  }

  /// @brief 先行グループのリストを返す．
  const std::vector<DichoGroup*>&
  pred_list() const
  {
    return mPredList;
  }

  /// @brief 先行グループを追加する．
  void
  add_pred(
    DichoGroup* group ///< [in] 追加するグループ
  )
  {
    mPredList.push_back(group);
  }

  /// @brief 故障グループの情報を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const
  {
    s << str()
      << std::endl;
    for ( auto group1: succ_list() ) {
      s << "  ==> " << group1->str() << std::endl;
    }
  }

  /// @brief 故障グループの内容を表す文字列を作る．
  std::string
  str(
    bool full = true ///< [in] 故障リストを全て表示する時 true
  ) const
  {
    std::ostringstream buf;
    buf << "[G#" << id() << "]:";
    for ( auto fault: fault_list() ) {
      buf << " " << fault.str();
      if ( !full ) {
	break;
      }
    }
    return buf.str();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 故障のリスト
  TpgFaultList mFaultList;

  // 後続グループのリスト
  std::vector<DichoGroup*> mSuccList;

  // 先行グループのリスト
  std::vector<DichoGroup*> mPredList;

};

END_NAMESPACE_DRUID

#endif // DICHOGROUP_H
