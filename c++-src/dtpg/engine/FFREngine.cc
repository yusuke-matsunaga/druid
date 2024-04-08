
/// @file FFREngine.cc
/// @brief FFREngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "FFREngine.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFREngine::FFREngine(
  const TpgNetwork& network,
  const TpgFFR* ffr,
  bool make_dchain,
  const string& ex_mode,
  const string& just_mode,
  const SatInitParam& init_param
) : DtpgEngine{network, ffr->root(), make_dchain,
	       ex_mode, just_mode, init_param}
{
}

// @brief デストラクタ
FFREngine::~FFREngine()
{
}

// @brief gen_pattern() で用いる検出条件を作る．
vector<SatLiteral>
FFREngine::gen_assumptions(
  const TpgFault* fault
)
{
  return {};
}

END_NAMESPACE_DRUID
