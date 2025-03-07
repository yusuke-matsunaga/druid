
/// @file DomChecker.cc
/// @brief DomChecker の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DomChecker.h"

#include "TpgNetwork.h"
#include "TpgFFR.h"
#include "TpgFault.h"


//#define DEBUG_DTPG

#define DEBUG_OUT cout
BEGIN_NONAMESPACE
#ifdef DEBUG_DTPG
int debug_dtpg = 1;
#else
const int debug_dtpg = 0;
#endif

END_NONAMESPACE


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
DomChecker::DomChecker(
  const TpgNetwork& network,
  const TpgFFR* ffr1,
  const TpgFFR* ffr2,
  const JsonValue& option
)
{
  mBdEnc1 = new BoolDiffEnc(ffr1->root(), option);
  mBdEnc2 = new BoolDiffEnc(ffr2->root(), option);
  StructEngine::Builder builder;
  builder.add_subenc(mBdEnc1);
  builder.add_subenc(mBdEnc2);
  mEngine = builder.new_obj(network, option);
}

// @brief デストラクタ
DomChecker::~DomChecker()
{
}

// @brief チェックをする．
bool
DomChecker::check(
  const TpgFault* fault1
)
{
  auto ffr_cond1 = fault1->ffr_propagate_condition();
  auto assumptions = mEngine->conv_to_literal_list(ffr_cond1);
  assumptions.push_back(mBdEnc1->prop_var());
  assumptions.push_back(~mBdEnc2->prop_var());
  return mEngine->solve(assumptions) == SatBool3::False;
}

END_NAMESPACE_DRUID
