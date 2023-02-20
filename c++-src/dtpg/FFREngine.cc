﻿
/// @file FFREngine.cc
/// @brief FFREngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "FFREngine.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFREngine::FFREngine(
  const TpgNetwork& network,
  FaultType fault_type,
  const TpgFFR& ffr,
  const SatSolverType& solver_type
) : DtpgEngine{network, fault_type, ffr.root(), true, solver_type}
{
}

// @brief デストラクタ
FFREngine::~FFREngine()
{
}

END_NAMESPACE_DRUID