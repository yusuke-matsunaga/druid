#ifndef TPGFAULTMGR_EX_H
#define TPGFAULTMGR_EX_H

/// @file TpgFaultMgr_Ex.h
/// @brief TpgFaultMgr_Ex のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFaultMgrImpl.h"


BEGIN_NAMESPACE_DRUID

class TpgFaultImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultMgr_Ex TpgFaultMgr_Ex.h "TpgFaultMgr_Ex.h"
/// @brief GateExaustive 故障を作るためのクラス
//////////////////////////////////////////////////////////////////////
class TpgFaultMgr_Ex :
  public TpgFaultMgrImpl
{
public:

  /// @brief コンストラクタ
  TpgFaultMgr_Ex(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) : TpgFaultMgrImpl{network}
  {
  }

  /// @brief デストラクタ
  ~TpgFaultMgr_Ex() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のタイプを返す．
  FaultType
  fault_type() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障を作る．
  void
  _gen_all_faults(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    unordered_set<SizeType>& rep_map ///< [in] 代表故障番号を表す辞書
  ) override;

  /// @brief 故障を作る．
  TpgFaultImpl*
  new_fault(
    const TpgNode* node,      ///< [in] ノード
    const string& node_name,  ///< [in] ノード名
    const vector<bool>& ivals ///< [in] 入力値のリスト
  );

};

END_NAMESPACE_DRUID

#endif // TPGFAULTMGR_EX_H
