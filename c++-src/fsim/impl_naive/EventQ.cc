
/// @file EventQ.cc
/// @brief EventQ の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "EventQ.h"
#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

BEGIN_NONAMESPACE
const bool debug = false;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// 故障シミュレーション用のイベントキューを表すクラス
//////////////////////////////////////////////////////////////////////

// @brief 初期化を行う．
void
EventQ::init(
  SizeType max_level,
  SizeType output_num,
  SizeType node_num
)
{
  mArray.clear();
  mArray.resize(max_level + 1, nullptr);

  mClearArray.clear();
  mClearArray.reserve(node_num);
  mFlipMaskArray.clear();
  mFlipMaskArray.resize(node_num, PV_ALL0);

  mCurLevel = 0;
  mNum = 0;
}

BEGIN_NONAMESPACE

inline
std::string
val_str2(
  FSIM_VALTYPE val
)
{
  std::ostringstream buf;
#if FSIM_VAL2
  for ( SizeType i = 0; i < 4; ++ i ) {
    PackedVal bit = 1UL << i;
    if ( val & bit ) {
      buf << " 1";
    }
    else {
      buf << " 0";
    }
  }
#elif FSIM_VAL3
  for ( SizeType i = 0; i < 4; ++ i ) {
    PackedVal bit = 1UL << i;
    if ( val.val0() & bit ) {
      buf << " 0";
    }
    else if ( val.val1() & bit ) {
      buf << " 1";
    }
    else {
      buf << " X";
    }
  }
#endif
  return buf.str();
}

END_NONAMESPACE

END_NAMESPACE_DRUID_FSIM
