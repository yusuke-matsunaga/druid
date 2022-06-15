#ifndef DTPGMFFC_H
#define DTPGMFFC_H

/// @file DtpgMFFC.h
/// @brief DtpgMFFC のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgMFFC DtpgMFFC.h "DtpgMFFC.h"
/// @brief MFFC 単位で DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgMFFC :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  DtpgMFFC(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    FaultType fault_type,	     ///< [in] 故障の種類
    const TpgMFFC& mffc,	     ///< [in] Justifier の種類を表す文字列
    const string& just_type,	     ///< [in] 故障伝搬の起点となる MFFC
    const SatSolverType& solver_type ///< [in] SATソルバの実装タイプ
    = SatSolverType()
  );

  /// @brief デストラクタ
  ~DtpgMFFC();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  /// @return 結果を返す．
  DtpgResult
  gen_pattern(
    const TpgFault* fault   ///< [in] 対象の故障
  );

  /// 使用禁止の宣言
  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  ///
  /// * FFR内の故障伝搬条件は含まない．
  NodeValList
  get_sufficient_condition() = delete;

  /// 使用禁止の宣言
  /// @brief 複数の十分条件を取り出す．
  ///
  /// * FFR内の故障伝搬条件は含まない．
  Expr
  get_sufficient_conditions() = delete;

  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  ///
  /// * root は MFFC モードの時 mRoot と異なる．
  /// * FFR内の故障伝搬条件は含まない．
  NodeValList
  get_sufficient_condition(
    const TpgNode* root  ///< [in] 対象の故障のあるFFRの根のノード
  );

  /// @brief 複数の十分条件を取り出す．
  ///
  /// * root は MFFC モードの時 mRoot と異なる．
  /// * FFR内の故障伝搬条件は含まない．
  Expr
  get_sufficient_conditions(
    const TpgNode* root  ///< [in] 対象の故障のあるFFRの根のノード
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief mffc 内の影響が root まで伝搬する条件のCNF式を作る．
  void
  gen_mffc_cnf();

  /// @brief 故障挿入回路のCNFを作る．
  void
  inject_fault(
    int elem_pos, ///< [in] 要素番号
    SatLiteral ovar ///< [in] ゲートの出力の変数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // FFR の根のリスト
  // [0] は MFFC の根でもある．
  vector<const TpgNode*> mElemArray;

  // 各FFRの根に反転イベントを挿入するための変数
  // サイズは mElemArray.size()
  vector<SatLiteral> mElemVarArray;

  // ノード番号をキーにしてFFR番号を入れる連想配列
  unordered_map<int, int> mElemPosMap;

};

END_NAMESPACE_DRUID

#endif // DTPGMFFC_H
