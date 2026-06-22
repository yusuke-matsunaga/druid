#ifndef EQDOMCANDMGR_H
#define EQDOMCANDMGR_H

/// @file EqDomCandMgr.h
/// @brief EqDomCandMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "EqDomCand.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/PackedVal.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class EqDomCandMgr EqDomCandMgr.h "EqDomCandMgr.h"
/// @brief 等価故障と支配故障候補のマネージャクラス
///
/// f1 が検出されて f2 が検出されないパタンがあった場合，
/// f1 が f2 を支配することはない．
/// この情報を管理するためのクラス
//////////////////////////////////////////////////////////////////////
class EqDomCandMgr
{
public:

  /// @brief コンストラクタ
  EqDomCandMgr(
    const TpgFaultList& fault_list ///< [in] 対象の故障リスト
  ) : mFaultList{fault_list}
  {
  }

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomCandMgr>
  new_obj(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomCandMgr>
  new_naive_mgr(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomCandMgr>
  new_dichotomy_mgr(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief 新しいオブジェクトを作る．
  static
  std::unique_ptr<EqDomCandMgr>
  new_dichotomy_mgr2(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief デストラクタ
  virtual
  ~EqDomCandMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  virtual
  bool
  update(
    const std::vector<PackedVal>& dpat_array ///< [in] 故障の検出状況のピットパタン
  ) = 0;

  /// @brief 結果を返す．
  virtual
  std::unique_ptr<EqDomCand>
  end(
    bool reduce ///< [in] 推移簡約を行う時 true
  ) const = 0;

  /// @brief 等価故障グループの候補を返す．
  virtual
  TpgFaultList
  eqcand(
    const TpgFault& fault ///< [in] 対象の故障
  ) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象の故障リストを返す．
  TpgFaultList
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 対象のネットワークを返す．
  TpgNetwork
  network() const
  {
    return fault_list().network();
  }

  /// @brief 故障番号の最大値を返す．
  SizeType
  max_fault_size() const
  {
    return network().max_fault_id();
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障のリスト
  TpgFaultList mFaultList;

};

END_NAMESPACE_DRUID

#endif // EQDOMCANDMGR_H
