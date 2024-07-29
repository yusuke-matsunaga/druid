
/// @file BSEnc.cc
/// @brief BSEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "BSEnc.h"

#include "TpgNetwork.h"
#include "ConeInfo.h"
#include "BSInfo.h"
#include "VidMap.h"
#include "GateEnc.h"


//#define DEBUG_DTPG

#define DEBUG_OUT cout

BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
const int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif
END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

// @brief CNF の生成を行う．
void
BSEnc::encode(
  SatSolver& solver,
  const ConeInfo& cone_info,
  const BSInfo& bs_info,
  const VidMap& gvar_map,
  VidMap& hvar_map
)
{

  //////////////////////////////////////////////////////////////////////
  // 1時刻前の正常回路の変数を作る．
  //////////////////////////////////////////////////////////////////////
  for ( auto node: bs_info.tfi_list() ) {
    auto hvar = solver.new_variable(true);

    hvar_map.set_vid(node, hvar);

    if ( debug_dtpg ) {
      DEBUG_OUT << node->str()
		<< ": hvar = " << hvar
		<< endl;
    }
  }

  //////////////////////////////////////////////////////////////////////
  /// DFFの出力と1時刻前のDFFの入力の値を一致させる．
  //////////////////////////////////////////////////////////////////////
  for ( auto node: cone_info.dff_output_list() ) {
    auto gvar = gvar_map(node);
    auto hvar = hvar_map(node->alt_node());
    solver.add_buffgate(gvar, hvar);
  }

  //////////////////////////////////////////////////////////////////////
  // 1時刻前の正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  GateEnc hval_enc{solver, hvar_map};
  for ( auto node: bs_info.tfi_list() ) {
    hval_enc.make_cnf(node);
  }
}

END_NAMESPACE_DRUID
