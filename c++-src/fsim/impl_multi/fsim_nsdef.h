#ifndef FSIM_NSDEF_H
#define FSIM_NSDEF_H

/// @file fsim_nsdef.h
/// @brief Fsim 用の名前空間の定義
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
// FSIM_VAL2: 2値の故障シミュレータ
// FSIM_VAL3: 3値の故障シミュレータ
//
// FSIM_COMBI: 組み合わせ回路用の故障シミュレータ
// FSIM_BSIDE: broad-side 方式の故障シミュレータ

#if FSIM_VAL2
#  if FSIM_COMBI
#    define FSIM_NAMESPACE nsFsimCombi2
#    define FSIM_CLASSNAME FsimCombi2
#  elif FSIM_BSIDE
#    define FSIM_NAMESPACE nsFsimBside2
#    define FSIM_CLASSNAME FsimBside2
#  else
#    error "Neither FSIM_COMBI nor FSIM_BSIDE are not set"
#  endif
#  define FSIM_VALTYPE PackedVal
#elif FSIM_VAL3
#  if FSIM_COMBI
#    define FSIM_NAMESPACE nsFsimCombi3
#    define FSIM_CLASSNAME FsimCombi3
#  elif FSIM_BSIDE
#    define FSIM_NAMESPACE nsFsimBside3
#    define FSIM_CLASSNAME FsimBside3
#  else
#    error "Neither FSIM_COMBI nor FSIM_BSIDE are not set"
#  endif
#  define FSIM_VALTYPE PackedVal3
#else
#  error "Neither FSIM_VAL2 nor FSIM_VAL3 are not set"
#endif

// 名前空間の定義
// 必ず先頭でインクルードしなければならない．

// namespace でネストするのがいやなので define マクロでごまかす．

#define BEGIN_NAMESPACE_DRUID_FSIM \
BEGIN_NAMESPACE_DRUID \
BEGIN_NAMESPACE(FSIM_NAMESPACE)

#define END_NAMESPACE_DRUID_FSIM \
END_NAMESPACE(FSIM_NAMESPACE) \
END_NAMESPACE_DRUID

#endif // FSIM_NSDEF_H
