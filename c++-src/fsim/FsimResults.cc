
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
)
{
}

// @brief 内容を指定するコンストラクタ
FsimResults::FsimResults(
  const std::shared_ptr<NetworkRep>& impl,
  const std::shared_ptr<FsimResultsRep>& src
) : TpgBase{impl},
    mArray{{src}}
{
}

// @brief 内容を指定するコンストラクタ
FsimResults::FsimResults(
  const std::shared_ptr<NetworkRep>& impl,
  const std::vector<std::shared_ptr<FsimResultsRep>>& src
) : TpgBase{impl},
    mArray{src}
{
}

// @brief テストベクタの総数を返す．
SizeType
FsimResults::tv_num() const
{
  return mArray.size();
}

// @brief 指定されたテストベクタ番号で検出された故障のリストを返す．
TpgFaultList
FsimResults::fault_list(
  SizeType tv_id
) const
{
  _check_tv_id(tv_id);
  auto& rep = mArray[tv_id];
  return TpgBase::fault_list(rep->fault_list());
}

// @brief 出力の故障伝搬状態を返す．
DiffBits
FsimResults::diffbits(
  SizeType tv_id,
  SizeType fault_id
) const
{
  _check_tv_id(tv_id);
  auto& rep = mArray[tv_id];
  return rep->diffbits(fault_id);
}

END_NAMESPACE_DRUID
