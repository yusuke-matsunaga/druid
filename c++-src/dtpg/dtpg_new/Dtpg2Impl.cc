
/// @file Dtpg2Impl.cc
/// @brief Dtpg2Impl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "Dtpg2Impl.h"

#include "TpgNetwork.h"
#include "TpgFault.h"

#include "BackTracer.h"
#include "ValMap.h"

#include "ym/RandGen.h"
#include "ym/RandCombiGen.h"
#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"

#include "../struct_sat/GateLitMap_vid.h"

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG2
int debug_dtpg2 = 1;
#else
const int debug_dtpg2 = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
Dtpg2Impl::Dtpg2Impl(const string& sat_type,
		     const string& sat_option,
		     ostream* sat_outp,
		     BackTracer& bt,
		     const TpgNetwork& network,
		     const TpgNode* root) :
  DtpgImpl(sat_type, sat_option, sat_outp, FaultType::TransitionDelay,
	   bt, network, root)
{
}

// @brief デストラクタ
Dtpg2Impl::~Dtpg2Impl()
{
}

// @brief XOR制約のための変数リストを作る．
// @return 変数の数を返す．
int
Dtpg2Impl::make_xor_list()
{
  // 対象の故障に関係する外部入力のリストを作る．
  mXorNodeList.clear();
  for (vector<const TpgNode*>::const_iterator p = cur_node_list().begin();
       p != cur_node_list().end(); ++ p) {
    const TpgNode* node = *p;
    if ( node->is_primary_input() ) {
      mXorNodeList.push_back(node);
    }
  }
  mXorNum1 = mXorNodeList.size();
  for (vector<const TpgNode*>::const_iterator p = prev_node_list().begin();
       p != prev_node_list().end(); ++ p) {
    const TpgNode* node = *p;
    if ( node->is_ppi() ) {
      mXorNodeList.push_back(node);
    }
  }
  mXorNum2 = mXorNodeList.size();
  return mXorNum2;
}

// @brief XOR制約を追加する．
void
Dtpg2Impl::add_xor_constraint(int num,
			      RandGen& rg)
{
  mXorLitList.clear();
  if ( num == mXorNum2 ) {
    // もともとの入力数が少ないときはランダムに最小項を選ぶ．
    for (int idx = 0; idx < mXorNum2; ++ idx) {
      const TpgNode* node = mXorNodeList[idx];
      SatVarId var;
      if ( idx < mXorNum1 ) {
	var = gvar(node);
      }
      else {
	var = hvar(node);
      }
      SatLiteral lit(var);
      mXorLitList.push_back(lit);
    }
    return;
  }

  double p = 0.5;
  //double p = 0.1;
  for (int i = 0; i < num; ++ i) {
    // p の確率で変数を選び，そのXORを作る．
    vector<SatVarId> var_list;
    for ( ; ; ) {
      for (int j = 0; j < mXorNum2; ++ j) {
	if ( rg.real1() < p ) {
	  const TpgNode* node = mXorNodeList[j];
	  if ( j < mXorNum1 ) {
	    SatVarId var = gvar(node);
	    var_list.push_back(var);
	  }
	  else {
	    SatVarId var = hvar(node);
	    var_list.push_back(var);
	  }
	}
      }
      if ( var_list.size() > 0 ) {
	break;
      }
    }
    SatLiteral xor_lit = make_xor(var_list, 0, var_list.size());
    if ( !solver().sane() ) {
      break;
    }
    mXorLitList.push_back(xor_lit);
  }
}

SatLiteral
Dtpg2Impl::make_xor(const vector<SatVarId>& var_list,
		    int start,
		    int end)
{
  if ( !solver().sane() ) {
    return kSatLiteralX;
  }

  int n = end - start;

  ASSERT_COND( n > 0 );

  if ( n == 1 ) {
    return SatLiteral(var_list[0]);
  }

  // n >= 2
  int n2 = start + (n / 2);
  SatLiteral lit1 = make_xor(var_list, start, n2);
  SatLiteral lit2 = make_xor(var_list, n2, end);
  SatVarId ovar = solver().new_variable();
  SatLiteral olit(ovar);
  solver().add_xorgate_rel(olit, lit1, lit2);
  return olit;
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
Dtpg2Impl::dtpg_with_xor(const TpgFault* fault,
			 int xor_assign,
			 NodeValList& nodeval_list,
			 DtpgStats& stats)
{
  if ( fault->tpg_onode()->ffr_root() != root_node() ) {
    cerr << "Error[Dtpg2Impl::dtpg()]: " << fault << " is not within mFfrRoot's FFR" << endl;
    cerr << " fautl->ffr_root() = " << fault->tpg_onode()->ffr_root()->name() << endl;
    return kB3X;
  }

  vector<SatLiteral> assumptions;
  for ( auto lit: mXorLitList ) {
    if ( xor_assign & (1U << i) ) {
      lit = ~lit;
    }
    assumptions.push_back(lit);
  }

  SatBool3 ans = solve(fault, assumptions, nodeval_list, stats);

  return ans;
}

END_NAMESPACE_DRUID
