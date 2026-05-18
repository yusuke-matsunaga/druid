
/// @file FsimResults.cc
/// @brief FsimResults の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "fsim/FsimResults.h"
#include "ResultsRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス FsimResults
//////////////////////////////////////////////////////////////////////

// @brief 内容を指定するコンストラクタ
FsimResults::FsimResults(
  const std::shared_ptr<NetworkRep>& impl,
  const std::shared_ptr<ResultsRep>& rep
) : TpgBase{impl},
    mRep{rep}
{
}

// @brief テストベクタの総数を返す．
SizeType
FsimResults::tv_num() const
{
  _check_rep();
  return mRep->tv_num();
}

// @brief 指定されたテストベクタ番号で検出された故障数を返す．
SizeType
FsimResults::fault_num(
  SizeType tv_id
) const
{
  _check_rep();
  return mRep->fault_num(tv_id);
}

// @brief 指定されたテストベクタ番号で検出された故障を返す．
TpgFault
FsimResults::fault(
  SizeType tv_id,
  SizeType pos
) const
{
  _check_rep();
  return TpgBase::fault(mRep->fid(tv_id, pos));
}

// @brief 指定されたテストベクタ番号で検出された故障のリストを返す．
TpgFaultList
FsimResults::fault_list(
  SizeType tv_id
) const
{
  _check_rep();
  return TpgBase::fault_list(mRep->fid_list(tv_id));
}

// @brief 指定されたテストベクタ番号で検出された故障の出力ごとの故障伝搬状態を返す．
DiffBits
FsimResults::diffbits(
  SizeType tv_id,
  const TpgFault& fault
) const
{
  _check_rep();
  return mRep->diffbits(tv_id, fault.id());
}

END_NAMESPACE_DRUID
