#ifndef MF_FAULTCOMP_H
#define MF_FAULTCOMP_H

/// @file MF_FaultComp.h
/// @brief MF_FaultComp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
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
  static
  vector<vector<const TpgFault*>>
  get_faults_list(
    const TpgNetwork& network,                  ///< [in] 対象のネットワーク
    const TpgNode* root,			///< [in] FFR の根のノード
    const vector<const TpgNode*>& input_list,	///< [in] FFR の葉のノードのリスト
    const vector<const TpgFault*>& fault_list,	///< [in] FFR内の代表故障のリスト
    SatSolverType solver_type = SatSolverType()	///< [in] SATソルバの実装タイプ
  );


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
