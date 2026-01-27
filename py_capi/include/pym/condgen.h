#ifndef PYM_CONDGEN_H
#define PYM_CONDGEN_H

/// @file condgen.h
/// @brief condgen の初期化関数のエクスポート
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

/// @brief condgen モジュールの初期化関数
extern "C" PyObject* PyInit_condgen();

END_NAMESPACE_DRUID

#endif // PYM_CONDGEN_H
