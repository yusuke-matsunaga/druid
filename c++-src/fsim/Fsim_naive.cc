﻿
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Fsim.h"
#include "FsimImpl.h"
#include "TestVector.h"


BEGIN_NAMESPACE_DRUID

namespace nsFsimNaiveCombi2 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network
  );
}

namespace nsFsimNaiveCombi3 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network
  );
}

namespace nsFsimNaiveBside2 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network
  );
}

namespace nsFsimNaiveBside3 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgNetwork& network
  );
}

BEGIN_NONAMESPACE

inline
std::unique_ptr<FsimImpl>
new_impl(
  const TpgNetwork& network,
  bool has_previous_state,
  bool has_x
)
{
  if ( has_x ) {
    // 3値バージョン
    if ( has_previous_state ) {
      return nsFsimNaiveBside3::new_Fsim(network);
    }
    else {
      return nsFsimNaiveCombi3::new_Fsim(network);
    }
  }
  else {
    // 2値バージョン
    if ( has_previous_state ) {
      return nsFsimNaiveBside2::new_Fsim(network);
    }
    else {
      return nsFsimNaiveCombi2::new_Fsim(network);
    }
  }
  ASSERT_NOT_REACHED;
  return nullptr;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
//////////////////////////////////////////////////////////////////////

// @brief 初期化
void
Fsim::initialize_naive(
  const TpgNetwork& network,
  bool has_previous_state,
  bool has_x
)
{
  mImpl = new_impl(network, has_previous_state, has_x);
}

END_NAMESPACE_DRUID
