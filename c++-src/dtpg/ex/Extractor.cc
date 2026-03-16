
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor.h"
#include "ExtSimple.h"
#include "ExtStd.h"
#include "ExData.h"
#include "dtpg/SuffCond.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE
int debug = 2;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス Extractor
//////////////////////////////////////////////////////////////////////

// Extractor の継承クラスを作る．
Extractor*
Extractor::new_impl(
  const JsonValue& option
)
{
  if ( option.is_null() ) {
    // デフォルトフォールバック
    return new ExtSimple;
  }
  if ( option.is_string() ) {
    auto mode = option.get_string();
    if ( mode == "simple" ) {
      return new ExtSimple;
    }
    if ( mode == "std" ) {
      return new ExtStd;
    }
    // 知らない型だった．
    std::ostringstream buf;
    buf << mode << ": unknown value for 'extractor'";
    throw std::invalid_argument{buf.str()};
  }

  // 文字列型ではなかった．
  std::ostringstream buf;
  buf << "value for 'extractor' should be a string or null";
  throw std::invalid_argument{buf.str()};
}

// @brief 値割り当てを１つ求める．
SuffCond
Extractor::operator()(
  const TpgNode& root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  ExData data(root, gvar_map, fvar_map, model);

  SuffCond min_cond;
  SizeType min_val = std::numeric_limits<SizeType>::max();
  for ( auto po: data.sensitized_output_list() ) {
    // 各出力に対する割り当てを求める．
    auto cond = backtrace(data, po);
    auto& assign_list = cond.main_cond();
    SizeType val = assign_list.size();
    if ( min_val > val ) {
      // 要素数が最小のものを選ぶ．
      min_val = val;
      min_cond = cond;
    }
  }
  return min_cond;
}

// @brief 値割り当てを１つ求める．
SuffCond
Extractor::operator()(
  const TpgNode& root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const TpgNode& output,
  const SatModel& model
)
{
  ExData data(root, gvar_map, fvar_map, model);
  return backtrace(data, output);
}

END_NAMESPACE_DRUID
