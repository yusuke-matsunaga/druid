
/// @file Fsim.cc
/// @brief Fsim の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim/Fsim.h"
#include "types/TpgFaultList.h"
#include "FsimImpl.h"


BEGIN_NAMESPACE_DRUID

namespace nsFsimCombi2 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgFaultList& fault_list,
    SizeType thread_num
  );
}

namespace nsFsimCombi3 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgFaultList& fault_list,
    SizeType thread_num
  );
}

namespace nsFsimBside2 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgFaultList& fault_list,
    SizeType thread_num
  );
}

namespace nsFsimBside3 {
  std::unique_ptr<FsimImpl> new_Fsim(
    const TpgFaultList& fault_list,
    SizeType thread_num
  );
}

BEGIN_NONAMESPACE

inline
std::unique_ptr<FsimImpl>
new_impl(
  const TpgFaultList& fault_list,
  bool has_previous_state,
  bool has_x,
  SizeType thread_num
)
{
  if ( has_x ) {
    // 3値バージョン
    if ( has_previous_state ) {
      return nsFsimBside3::new_Fsim(fault_list, thread_num);
    }
    else {
      return nsFsimCombi3::new_Fsim(fault_list, thread_num);
    }
  }
  else {
    // 2値バージョン
    if ( has_previous_state ) {
      return nsFsimBside2::new_Fsim(fault_list, thread_num);
    }
    else {
      return nsFsimCombi2::new_Fsim(fault_list, thread_num);
    }
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
//////////////////////////////////////////////////////////////////////

// @brief 初期化
void
Fsim::initialize_multi(
  const TpgFaultList& fault_list,
  bool has_previous_state,
  bool has_x,
  SizeType thread_num
)
{
  mImpl = new_impl(fault_list, has_previous_state, has_x, thread_num);
}

END_NAMESPACE_DRUID
