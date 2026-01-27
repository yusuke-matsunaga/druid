#ifndef PYM_DRUID_H
#define PYM_DRUID_H

/// @file druid.h
/// @brief druid の初期化関数のエクスポート
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

/// @brief druid モジュールの初期化関数
extern "C" PyObject* PyInit_druid();

END_NAMESPACE_DRUID

#endif // PYM_DRUID_H
