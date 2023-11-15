#ifndef FSIM2_NSDEF_H
#define FSIM2_NSDEF_H

/// @file fsim2_nsdef.h
/// @brief Fsim2 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

// ちょっと特殊なマクロ定義
// 2値/3値の切り替えと1時刻前の値を持つかどうかの切り替えを
// マクロの定義で行って計4つのクラスを1つのソースファイル
// から生成する．
//
// FSIM2_VAL2: 2値の故障シミュレータ
// FSIM2_VAL3: 3値の故障シミュレータ
//
// FSIM2_COMBI: 組み合わせ回路用の故障シミュレータ
// FSIM2_BSIDE: broad-side 方式の故障シミュレータ

#if FSIM2_VAL2
#  if FSIM2_COMBI
#    define FSIM2_NAMESPACE nsFsim2Combi2
#    define FSIM2_CLASSNAME Fsim2Combi2
#  elif FSIM2_BSIDE
#    define FSIM2_NAMESPACE nsFsim2Bside2
#    define FSIM2_CLASSNAME Fsim2Bside2
#  else
#    error "Neither FSIM2_COMBI nor FSIM2_BSIDE are not set"
#  endif
#  define FSIM2_VALTYPE PackedVal
#elif FSIM2_VAL3
#  if FSIM2_COMBI
#    define FSIM2_NAMESPACE nsFsim2Combi3
#    define FSIM2_CLASSNAME Fsim2Combi3
#  elif FSIM2_BSIDE
#    define FSIM2_NAMESPACE nsFsim2Bside3
#    define FSIM2_CLASSNAME Fsim2Bside3
#  else
#    error "Neither FSIM2_COMBI nor FSIM2_BSIDE are not set"
#  endif
#  define FSIM2_VALTYPE PackedVal3
#else
#  error "Neither FSIM2_VAL2 nor FSIM2_VAL3 are not set"
#endif

// 名前空間の定義
// 必ず先頭でインクルードしなければならない．

// namespace でネストするのがいやなので define マクロでごまかす．

#define BEGIN_NAMESPACE_DRUID_FSIM2 \
BEGIN_NAMESPACE_DRUID \
BEGIN_NAMESPACE(FSIM2_NAMESPACE)

#define END_NAMESPACE_DRUID_FSIM2 \
END_NAMESPACE(FSIM2_NAMESPACE) \
END_NAMESPACE_DRUID

#endif // FSIM22_NSDEF_H
