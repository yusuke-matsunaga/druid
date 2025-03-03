#ifndef CNFGENCOVER_H
#define CNFGENCOVER_H

/// @file CnfGenCover.h
/// @brief CnfGenCover のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "CnfGenNaive.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class CnfGenCover CnfGenCover.h "CnfGenCover.h"
/// @brief カバーをCNFに変換する
//////////////////////////////////////////////////////////////////////
class CnfGenCover :
  public CnfGenNaive
{
public:

  /// @brief コンストラクタ
  CnfGenCover() = default;

  /// @brief デストラクタ
  ~CnfGenCover() = default;

};

END_NAMESPACE_DRUID

#endif // CNFGENCOVER_H
