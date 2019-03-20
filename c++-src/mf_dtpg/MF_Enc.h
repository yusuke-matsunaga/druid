#ifndef MF_ENC_H
#define MF_ENC_H

/// @file MF_Enc.h
/// @brief MF_Enc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


#include "TpgNetwork.h"

#include "ym/sat.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MF_Enc MF_Enc.h "MF_Enc.h"
/// @brief 多重故障回路のCNFを作るクラス
//////////////////////////////////////////////////////////////////////
class MF_Enc
{
public:

public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief FFR に対する正常回路を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] root FFR の根のノード
  /// @param[in] input_list FFR の葉のノードのリスト
  /// @param[in] input_vars FFR の入力に対応する変数のリスト
  /// @param[in] output_var FFR の出力に対応する変数
  ///
  /// * input_vars の順番は input_list の順番と同じ
  static
  void
  make_good_FFR(SatSolver& solver,
		const TpgNode* root,
		const vector<const TpgNode*>& input_list,
		const vector<SatVarId>& input_vars,
		SatVarId output_var);

  /// @brief FFR に対する故障回路を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] network 対象のネットワーク
  /// @param[in] root FFR の根のノード
  /// @param[in] input_list FFR の葉のノードのリスト
  /// @param[in] input_vars FFR の入力に対応する変数のリスト
  /// @param[in] output_var FFR の出力に対応する変数
  /// @param[in] fault_list FFR内の代表故障のリスト
  /// @param[in] fault_vars FFR内の代表故障に対応する変数のリスト
  ///
  /// * input_vars の順番は input_list の順番と同じ
  /// * fault_vars の順番は fault_list の順番と同じ
  static
  void
  make_faulty_FFR(SatSolver& solver,
		  const TpgNetwork& network,
		  const TpgNode* root,
		  const vector<const TpgNode*>& input_list,
		  const vector<SatVarId>& input_vars,
		  SatVarId output_var,
		  const vector<const TpgFault*>& fault_list,
		  const vector<SatVarId>& fault_vars);


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

#endif // MF_ENC_H
