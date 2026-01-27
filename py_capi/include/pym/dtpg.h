#ifndef PYM_DTPG_H
#define PYM_DTPG_H

/// @file dtpg.h
/// @brief dtpg の初期化関数のエクスポート
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

/// @brief dtpg モジュールの初期化関数
extern "C" PyObject* PyInit_dtpg();

END_NAMESPACE_DRUID

#endif // PYM_DTPG_H
