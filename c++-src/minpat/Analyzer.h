#ifndef ANALYZER_H
#define ANALYZER_H

/// @file Analyzer.h
/// @brief Analyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DtpgEngine.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_DRUID

class FaultInfo;

//////////////////////////////////////////////////////////////////////
/// @class Analyzer Analyzer.h "Analyzer.h"
/// @brief 故障の情報を解析するクラス
//////////////////////////////////////////////////////////////////////
class Analyzer
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault_type 故障の種類
  Analyzer(const TpgNetwork& network,
	   FaultType fault_type);

  /// @brief デストラクタ
  ~Analyzer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の支配関係を調べて故障リストを縮約する．
  /// @param[inout] fault_list 対象の故障リスト
  /// @param[in] algorithm アルゴリズム
  void
  fault_reduction(vector<const TpgFault*>& fault_list,
		  const string& algorithm);

  /// @brief 検出可能故障リストを作る．
  /// @param[out] fi_list 故障情報のリスト
  void
  gen_fault_list(const vector<bool>& mark,
		 vector<FaultInfo*>& fi_list);

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  /// @param[inout] fi_list 故障情報のリスト
  void
  dom_reduction1(vector<FaultInfo*>& fi_list);

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  /// @param[inout] fi_list 故障情報のリスト
  void
  dom_reduction2(vector<FaultInfo*>& fi_list);

  /// @brief 初期化する
  /// @param[in] loop_limit 反復回数の上限
  void
  init(int loop_limit);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の検出条件の解析を行う．
  /// @param[in] dtpg DTPGエンジン
  /// @param[in] fault 対象の故障
  /// @param[in] loop_limit 反復回数の上限
  /// @return FaultInfo を返す．
  ///
  /// fault が検出不能の場合には nullptr を返す．
  FaultInfo*
  analyze_fault(DtpgFFR& dtpg,
		const TpgFault* fault,
		int loop_limit);

  /// @brief 論理式に含まれるキューブを求める．
  /// @param[in] expr 論理式
  NodeValList
  common_cube(const Expr& expr);

  /// @brief 必要割り当てに従って論理式を簡単化する．
  /// @param[in] expr 論理式
  /// @param[in] mand_cond 必要割り当て
  /// @return 簡単化した論理式を返す．
  Expr
  restrict(const Expr& expr,
	   const NodeValList& mand_cond);

  /// @brief restrict の下請け関数
  /// @param[in] expr 論理式
  /// @param[in] val_map 割り当てマップ
  Expr
  _restrict_sub(const Expr& expr,
		const unordered_map<VarId, bool>& val_map);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // 故障情報のリスト
  vector<FaultInfo*> mFaultInfoList;

};

END_NAMESPACE_DRUID

#endif // ANALYZER_H
