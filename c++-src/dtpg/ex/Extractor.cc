
/// @file Extractor.cc
/// @brief Extractor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Extractor.h"
#include "ExtSimple.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

// ExImpl の継承クラスを作る．
ExImpl*
new_impl(
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
    // 知らない型だった．
    ostringstream buf;
    buf << mode << ": unknown value for 'extractor'";
    throw std::invalid_argument{buf.str()};
  }

  // 文字列型ではなかった．
  ostringstream buf;
  buf << "value for 'extractor' should be a string or null";
  throw std::invalid_argument{buf.str()};
  // ダミー
  return nullptr;
}


//////////////////////////////////////////////////////////////////////
// クラス Extractor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Extractor::Extractor(
  const JsonValue& option
) : mImpl{new_impl(option)}
{
}

// @brief デストラクタ
Extractor::~Extractor()
{
}

// @brief 値割り当てを１つ求める．
NodeTimeValList
Extractor::operator()(
  const TpgNode* root,
  const VidMap& gvar_map,
  const VidMap& fvar_map,
  const SatModel& model
)
{
  return mImpl->get_assignment(root, gvar_map, fvar_map, model);
}

END_NAMESPACE_DRUID
