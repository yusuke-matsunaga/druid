#ifndef PYM_TPG_TYPES_H
#define PYM_TPG_TYPES_H

/// @file tpg_types.h
/// @brief tpg_types の初期化関数のエクスポート
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

/// @brief tpg_types モジュールの初期化関数
extern "C" PyObject* PyInit_tpg_types();

END_NAMESPACE_DRUID

#endif // PYM_TPG_TYPES_H
