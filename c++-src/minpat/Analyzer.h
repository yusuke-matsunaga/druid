#ifndef ANALYZER_H
#define ANALYZER_H

/// @file Analyzer.h
/// @brief Analyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
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
  Analyzer(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~Analyzer();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の支配関係を調べて故障リストを縮約する．
  void
  fault_reduction(
    vector<const TpgFault*>& fault_list, ///< [inout] 対象の故障リスト
    const string& algorithm		 ///< [in] アルゴリズム
  );

  /// @brief 検出可能故障リストを作る．
  vector<FaultInfo*>
  gen_fault_list(
    const vector<bool>& mark
  );

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction1(
    vector<FaultInfo*>& fi_list ///< [inout] 故障情報のリスト
  );

  /// @brief 異なる FFR 間の支配故障の簡易チェックを行う．
  void
  dom_reduction2(
    vector<FaultInfo*>& fi_list ///< [inout] 故障情報のリスト
  );

  /// @brief 初期化する
  void
  init(
    int loop_limit ///< [in] 反復回数の上限
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の検出条件の解析を行う．
  /// @return FaultInfo を返す．
  ///
  /// fault が検出不能の場合には nullptr を返す．
  FaultInfo*
  analyze_fault(
    DtpgFFR& dtpg,         ///< [in] DTPGエンジン
    const TpgFault* fault, ///< [in] 対象の故障
    int loop_limit	   ///< [in] 反復回数の上限
  );

  /// @brief 論理式に含まれるキューブを求める．
  NodeValList
  common_cube(
    const Expr& expr ///< [in] 論理式
  );

  /// @brief 必要割り当てに従って論理式を簡単化する．
  /// @return 簡単化した論理式を返す．
  Expr
  restrict(
    const Expr& expr,            ///< [in] 論理式
    const NodeValList& mand_cond ///< [in] 必要割り当て
  );

  /// @brief restrict の下請け関数
  Expr
  _restrict_sub(
    const Expr& expr,                         ///< [in] 論理式
    const unordered_map<SizeType, bool>& val_map ///< [in] 割り当てマップ
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障情報のリスト
  vector<FaultInfo*> mFaultInfoList;

};

END_NAMESPACE_DRUID

#endif // ANALYZER_H
