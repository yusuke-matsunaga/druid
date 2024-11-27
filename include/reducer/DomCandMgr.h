#ifndef DOMCANDMGR_H
#define DOMCANDMGR_H

/// @file DomCandMgr.h
/// @brief DomCandMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DomCandMgr DomCandMgr.h "DomCandMgr.h"
/// @brief シミュレーションを用いた支配関係の候補リストを保持するクラス
//////////////////////////////////////////////////////////////////////
class DomCandMgr
{
public:

  /// @brief コンストラクタ
  DomCandMgr(
    const TpgNetwork& network ///< [in] ネットワーク
  );

  /// @brief デストラクタ
  ~DomCandMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 支配関係の候補を作る．
  void
  generate(
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障のリスト
    const vector<TestVector>& tv_list,         ///< [in] テストベクタのリスト
    SizeType limit                             ///< [in] シミュレーション用のパラメータ
  );

  /// @brief 被支配故障の候補リストを返す．
  const vector<const TpgFault*>&
  dom_cand_list(
    const TpgFault* fault ///< [in] 対象の故障
  ) const
  {
    return mDomCandListArray[fault->id()];
  }

  /// @brief 支配故障の候補リストを返す．
  const vector<const TpgFault*>&
  rev_cand_list(
    const TpgFault* fault ///< [in] 対象の故障
  ) const
  {
    return mRevCandListArray[fault->id()];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const TpgNetwork& mNetwork;

  // 被支配故障の候補リスト
  // キーは故障番号
  vector<vector<const TpgFault*>> mDomCandListArray;

  // mDomCandlist の逆のリスト
  // キーは故障番号
  vector<vector<const TpgFault*>> mRevCandListArray;
};

END_NAMESPACE_DRUID

#endif // DOMCANDMGR_H
