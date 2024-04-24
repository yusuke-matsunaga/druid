#ifndef DTPGENGINE2_H
#define DTPGENGINE2_H

/// @file DtpgEngine2.h
/// @brief DtpgEngine2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "TpgNetwork.h"
#include "TpgNode.h"
#include "DtpgStats.h"
#include "FaultType.h"
#include "Val3.h"
#include "VidMap.h"
#include "NodeValList.h"
#include "Justifier.h"

#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine2 DtpgEngine2.h "DtpgEngine2.h"
/// @brief DTPG の基本的な処理を行うクラス
//////////////////////////////////////////////////////////////////////
class DtpgEngine2
{
public:

  /// @brief コンストラクタ
  DtpgEngine2(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgNode* root,	   ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option        ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgEngine2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief solve() が成功した時にテストパタンを生成する．
  TestVector
  gen_pattern(
    const TpgFault* fault ///< [in] 故障
  )
  {
    auto assign_list = get_sufficient_condition(fault);
    return justify(assign_list);
  }

  /// @brief 十分条件を取り出す．
  /// @return 十分条件を表す割当リストを返す．
  NodeValList
  get_sufficient_condition(
    const TpgFault* fault ///< [in] 故障
  );

  /// @brief 十分条件からテストベクタを作る．
  TestVector
  justify(
    const NodeValList& assign_list ///< [in] 十分条件
  );


public:
  //////////////////////////////////////////////////////////////////////
  // ネットワークの構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号の最大値を返す．
  SizeType
  max_node_id() const
  {
    return network().node_num();
  }


public:
  //////////////////////////////////////////////////////////////////////
  // SATソルバ関係の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

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

  /// @brief SATソルバに変数を割り当てる．
  SatLiteral
  new_variable()
  {
    return solver().new_variable(true);
  }

  /// @brief SATソルバに節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] 節を構成するリテラルのリスト
  )
  {
    solver().add_clause(lits);
  }

  /// @brief テストパタン生成を行う．
  SatBool3
  solve(
    const TpgFault* fault ///< [in] 故障
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief gen_pattern() で用いる追加の検出条件を作る．
  ///
  /// デフォルトでは空を返す．
  virtual
  vector<SatLiteral>
  gen_assumptions(
    const TpgFault* fault ///< [in] 対象の故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // extractor のオプション
  JsonValue mExOpt;

  // justifier
  Justifier mJustifier;

  // 正常値の変数マップ
  VidMap mGvarMap;

  // 故障値の変数マップ
  VidMap mFvarMap;

};

END_NAMESPACE_DRUID

#endif // DTPGENGINE2_H
