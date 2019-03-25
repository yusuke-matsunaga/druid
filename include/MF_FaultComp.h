#ifndef MF_FAULTCOMP_H
#define MF_FAULTCOMP_H

/// @file MF_FaultComp.h
/// @brief MF_FaultComp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/sat.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MF_FaultComp MF_FaultComp.h "MF_FaultComp.h"
/// @brief FFR 単位で多重故障の代表故障を求めるクラス
//////////////////////////////////////////////////////////////////////
class MF_FaultComp
{
public:


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR 内の多重故障リストを求める．
  /// @param[in] network 対象のネットワーク
  /// @param[in] root FFR の根のノード
  /// @param[in] input_list FFR の葉のノードのリスト
  /// @param[in] fault_list FFR内の代表故障のリスト
  /// @param[in] solver_type SATソルバの実装タイプ
  static
  vector<vector<const TpgFault*>>
  get_faults_list(const TpgNetwork& network,
		  const TpgNode* root,
		  const vector<const TpgNode*>& input_list,
		  const vector<const TpgFault*>& fault_list,
		  SatSolverType solver_type = SatSolverType());


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_DRUID

#endif // MF_FAULTCOMP_H
