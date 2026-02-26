
/// @file FsimResults.cc
/// @brief FsimResults の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "fsim/FsimResults.h"
#include "FsimResultsRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FsimResults
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
FsimResults::FsimResults(
) : mPtr{new FsimResultsRep}
{
}

// @brief 内容を指定するコンストラクタ
FsimResults::FsimResults(
  const std::shared_ptr<FsimResultsRep>& src
) : mPtr{src}
{
}

// @brief テストベクタの総数を返す．
SizeType
FsimResults::tv_num() const
{
  return mPtr->tv_num();
}

// @brief 指定されたテストベクタ番号で検出された故障番号のリストを返す．
std::vector<SizeType>
FsimResults::fault_list(
  SizeType tv_id
) const
{
  return mPtr->fault_list(tv_id);
}

// @brief 出力の故障伝搬状態を返す．
DiffBits
FsimResults::diffbits(
  SizeType tv_id,
  SizeType fault_id
) const
{
  return mPtr->diffbits(tv_id, fault_id);
}

END_NAMESPACE_DRUID
