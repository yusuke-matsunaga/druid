#ifndef DOMMGR_H
#define DOMMGR_H

/// @file DomMgr.h
/// @brief DomMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "RedMgr.h"
#include "types/TpgFaultList.h"


BEGIN_NAMESPACE_DRUID

class EqGroupMgr;

//////////////////////////////////////////////////////////////////////
/// @class DomMgr DomMgr.h "DomMgr.h"
/// @brief 支配故障候補の情報を表すクラス
///
/// 等価故障のチェックが済んでいるものと仮定しているので
/// グループ単位ではなく故障単位で表す．
//////////////////////////////////////////////////////////////////////
class DomMgr :
  public RedMgr
{
public:

  /// @brief コンストラクタ
  DomMgr(
    const EqGroupMgr& eqmgr ///< [in] 等価故障の情報
  );

  /// @brief デストラクタ
  ~DomMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 支配故障の候補リストを得る．
  TpgFaultList&
  cand_list(
    const TpgFault& fault ///< [in] 対象の故障
  )
  {
    return mCandListArray[fault.id()];
  }

  /// @brief 故障シミュレーションの結果で候補リストを更新する．
  bool
  update(
    const std::vector<TestVector>& tv_list ///< [in] テストベクタのリスト
  );

  /// @brief 対象の故障に対する支配故障を設定する．
  void
  set_dominator(
    const TpgFault& fault,    ///< [in] 対象の故障
    const TpgFault& dom_fault ///< [in] 支配故障
  )
  {
    RedMgr::set_dominator(fault, dom_fault);
    mCandListArray[fault.id()].clear();
  }

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障番号をキーにして支配故障の候補のリストを持つ配列
  std::vector<TpgFaultList> mCandListArray;

};

END_NAMESPACE_DRUID

#endif // DOMMGR_H
