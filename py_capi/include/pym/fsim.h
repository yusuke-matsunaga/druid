#ifndef PYM_FSIM_H
#define PYM_FSIM_H

/// @file fsim.h
/// @brief fsim の初期化関数のエクスポート
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

/// @brief fsim モジュールの初期化関数
extern "C" PyObject* PyInit_fsim();

END_NAMESPACE_DRUID

#endif // PYM_FSIM_H
