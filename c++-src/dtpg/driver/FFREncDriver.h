#ifndef FFRENCDRIVER_H
#define FFRENCDRIVER_H

/// @file FFREncDriver.h
/// @brief FFREncDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriverImpl.h"
#include "StructEnc.h"
#include "Justifier.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFREncDriver FFREncDriver.h "FFREncDriver.h"
/// @brief FFREnc を用いたドライバ
//////////////////////////////////////////////////////////////////////
class FFREncDriver :
  public DtpgDriverImpl
{
public:

  /// @brief コンストラクタ
  FFREncDriver(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const TpgFFR* ffr,	       ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFREncDriver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障を検出する条件を求める．
  ///
  /// - f はコンストラクタで指定した FFR 内の故障でなければならない．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief テストパタン生成を行う．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const override;

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 値割り当てをリテラルに変換する．
  SatLiteral
  conv_to_literal(
    NodeVal node_val ///< [in] 値割当リスト
  );

  /// @brief 値割り当てをリテラルのリストに変換する．
  vector<SatLiteral>
  conv_to_literal_list(
    const NodeValList& assign_list ///< [in] 値の割り当てリスト
  )
  {
    vector<SatLiteral> ans_list;
    add_to_literal_list(assign_list, ans_list);
    return ans_list;
  }

  /// @brief 値割り当てをリテラルのリストに追加する．
  void
  add_to_literal_list(
    const NodeValList& assign_list, ///< [in] 値の割り当てリスト
    vector<SatLiteral>& lit_list    ///< [out] 変換したリテラルを追加するリスト
  );



private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 正常値の変数マップ
  VidMap mGvarMap;

  // 故障値の変数マップ
  VidMap mFvarMap;

  // 位置時刻前の正常値の変数マップ
  VidMap mHvarMap;

  // justifier
  Justifier mJustifier;

  // extractor のオプション
  JsonValue mExOpt;

  // CNFの生成時間
  double mCnfTime;

};

END_NAMESPACE_DRUID

#endif // FFRENCDRIVER_H
