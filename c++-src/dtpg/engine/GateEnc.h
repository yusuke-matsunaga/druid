#ifndef GATEENC_H
#define GATEENC_H

/// @file GateEnc.h
/// @brief GateEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
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
  GateEnc(
    SatSolver& solver,   ///< [in] SATソルバ
    const VidMap& varmap ///< [in] 変数番号のマップ
  );

  /// @brief デストラクタ
  ~GateEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの入出力の関係を表すCNF式を作る．
  void
  make_cnf(
    const TpgNode& node ///< [in] 対象のノード
  );

  /// @brief ノードの入出力の関係を表すCNF式を作る．
  ///
  /// 出力の変数番号のみ指定するバージョン
  void
  make_cnf(
    const TpgNode& node, ///< [in] 対象のノード
    SatLiteral ovar	 ///< [in] 出力の変数リテラル
  );

  /// @brief ノードの入出力の関係を表すCNF式のサイズを見積もる．
  static
  CnfSize
  calc_cnf_size(
    const TpgNode& node  ///< [in] 対象のノード
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードに対応するリテラルを返す．
  SatLiteral
  lit(
    const TpgNode& node ///< [in] ノード
  );


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
