
/// @file DPat.cc
/// @brief DPat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "DPat.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス DPat
//////////////////////////////////////////////////////////////////////

// @brief right に包含されていたら true を返す．
bool
DPat::check_contained(
  const DPat& right
) const
{
  auto n = mBody.size();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto p1 = mBody[i];
    auto p2 = right.mBody[i];
    if ( (p1 & p2) != p1 ) {
      return false;
    }
  }
  return true;
}

// @brief 1のビット数を数える．
SizeType
DPat::count_ones() const
{
  SizeType ans = 0;
  for ( auto pat: mBody ) {
    ans += DRUID_NAMESPACE::count_ones(pat);
  }
  return ans;
}

// @brief ハッシュ値を計算する．
SizeType
DPat::hash() const
{
  SizeType val = 0;
  for ( auto pat: mBody ) {
    val = val * 13 + pat;
  }
  return (val * val) >> 16;
}

// @brief 内容を表す文字列を返す．
std::string
DPat::to_str() const
{
  std::ostringstream buf;
  for ( SizeType i = 0; i < size(); ++ i ) {
    if ( get(i) ) {
      buf << '1';
    }
    else {
      buf << '0';
    }
  }
  return buf.str();
}

END_NAMESPACE_DRUID
