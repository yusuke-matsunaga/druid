
/// @file TpgLogicXNOR.cc
/// @brief TpgLogicXNOR[x] の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicXNOR.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgLogicXNOR2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
TpgLogicXNOR2::TpgLogicXNOR2(ymuint id,
			     const vector<TpgNode*>& fanin_list) :
  TpgLogic2(id, fanin_list)
{
}

// @brief デストラクタ
TpgLogicXNOR2::~TpgLogicXNOR2()
{
}

// @brief ゲートタイプを得る．
//
// is_logic() が false の場合の返り値は不定
GateType
TpgLogicXNOR2::gate_type() const
{
  return kGateXNOR;
}

// @brief controling value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicXNOR2::cval() const
{
  return kValX;
}

// @brief noncontroling valueを得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicXNOR2::nval() const
{
  return kValX;
}

// @brief controling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicXNOR2::coval() const
{
  return kValX;
}

// @brief noncontroling output value を得る．
//
// is_logic() が false の場合の返り値は不定
// ない場合は kValX を返す．
Val3
TpgLogicXNOR2::noval() const
{
  return kValX;
}

// @brief 入出力の関係を表す CNF 式を生成する．
// @param[in] solver SAT ソルバ
// @param[in] lit_map 入出力とリテラルの対応マップ
void
TpgLogicXNOR2::make_cnf(SatSolver& solver,
			const GateLitMap& lit_map) const
{
  SatLiteral ilit0 = lit_map.input(0);
  SatLiteral ilit1 = lit_map.input(1);
  SatLiteral olit  = lit_map.output();

  solver.add_xnorgate_rel(olit, ilit0, ilit1);
}

// @brief 入出力の関係を表す CNF 式を生成する(故障あり)．
// @param[in] solver SAT ソルバ
// @param[in] fpos 故障のある入力位置
// @param[in] fval 故障値 ( 0 / 1 )
// @param[in] lit_map 入出力とリテラルの対応マップ
//
// こちらは入力に故障を仮定したバージョン
void
TpgLogicXNOR2::make_faulty_cnf(SatSolver& solver,
			       ymuint fpos,
			       int fval,
			       const GateLitMap& lit_map) const
{
  ymuint pos = (fpos == 0) ? 1 : 0;
  SatLiteral ilit0 = lit_map.input(pos);
  SatLiteral olit  = lit_map.output();
  if ( fval == 0 ) {
    solver.add_eq_rel(~olit, ilit0);
  }
  else {
    solver.add_neq_rel(~olit, ilit0);
  }
}

END_NAMESPACE_YM_SATPG
