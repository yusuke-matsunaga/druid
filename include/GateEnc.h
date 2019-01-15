#ifndef GATEENC_H
#define GATEENC_H

/// @file GateEnc.h
/// @brief GateEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateEnc GateEnc.h "GateEnc.h"
/// @brief TpgNode の入出力の関係を表す CNF 式を作るクラス
//////////////////////////////////////////////////////////////////////
class GateEnc
{
public:

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  /// @param[in] varmap 変数番号のマップ
  GateEnc(SatSolver& solver,
	  const VidMap& varmap);

  /// @brief デストラクタ
  ~GateEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの入出力の関係を表すCNF式を作る．
  /// @param[in] node 対象のノード
  void
  make_cnf(const TpgNode* node);

  /// @brief ノードの入出力の関係を表すCNF式を作る．
  /// @param[in] node 対象のノード
  /// @param[in] ovar 出力の変数番号
  ///
  /// 出力の変数番号のみ指定するバージョン
  void
  make_cnf(const TpgNode* node,
	   SatVarId ovar);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードに対応するリテラルを返す．
  SatLiteral
  lit(const TpgNode* node);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

  // 変数番号のマップ
  const VidMap& mVarMap;

};

END_NAMESPACE_DRUID

#endif // GATEENC_H
