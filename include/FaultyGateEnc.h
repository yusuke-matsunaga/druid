#ifndef FAULTYGATEENC_H
#define FAULTYGATEENC_H

/// @file FaultyGateEnc.h
/// @brief FaultyGateEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultyGateEnc FaultyGateEnc.h "FaultyGateEnc.h"
/// @brief 故障のある TpgNode の入出力の関係を表す CNF 式を作るクラス
//////////////////////////////////////////////////////////////////////
class FaultyGateEnc
{
public:

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  /// @param[in] varmap 変数番号のマップ
  /// @param[in] fault 対象の故障
  FaultyGateEnc(SatSolver& solver,
		const VidMap& varmap,
		const TpgFault* fault);

  /// @brief デストラクタ
  ~FaultyGateEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの入出力の関係を表すCNF式を作る．
  void
  make_cnf();

  /// @brief ノードの入出力の関係を表すCNF式を作る．
  /// @param[in] ovar 出力の変数番号
  ///
  /// 出力の変数番号のみ指定するバージョン
  void
  make_cnf(SatVarId ovar);


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

  // 故障
  const TpgFault* mFault;

};

END_NAMESPACE_DRUID

#endif // FAULTYGATEENC_H
