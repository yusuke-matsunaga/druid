
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
  FsimResultsRep* src
) : TpgBase{impl},
    mArray{src}
{
}

// @brief 内容を指定するコンストラクタ
FsimResults::FsimResults(
  const std::shared_ptr<NetworkRep>& impl,
  const std::vector<FsimResultsRep*>& src_list
) : TpgBase{impl},
    mArray{src_list}
{
}

// @brief コピーコンストラクタ
FsimResults::FsimResults(
  const FsimResults& src
) : TpgBase{src}
{
  _copy(src);
}

// @brief 代入演算子
FsimResults&
FsimResults::operator=(
  const FsimResults& src
)
{
  if ( this != &src ) {
    TpgBase::operator=(src);
    _clear();
    _copy(src);
  }
  return *this;
}

// @brief デストラクタ
FsimResults::~FsimResults()
{
  _clear();
}

// @brief クリアする．
void
FsimResults::_clear()
{
  for ( auto rep: mArray ) {
    delete rep;
  }
  mArray.clear();
}

// @brief コピーする
void
FsimResults::_copy(
  const FsimResults& src
)
{
  mArray.reserve(src.mArray.size());
  for ( auto src_rep: src.mArray ) {
    auto rep = new FsimResultsRep(*src_rep);
    mArray.push_back(rep);
  }
}

// @brief テストベクタの総数を返す．
SizeType
FsimResults::tv_num() const
{
  return mArray.size();
}

// @brief 指定されたテストベクタ番号で検出された故障数を返す．
SizeType
FsimResults::det_num(
  SizeType tv_id
) const
{
  _check_tv_id(tv_id);
  auto rep = mArray[tv_id];
  return rep->det_num();
}

// @brief 指定されたテストベクタ番号で検出された故障を返す．
TpgFault
FsimResults::fault(
  SizeType tv_id,
  SizeType pos
) const
{
  _check_tv_id(tv_id);
  auto rep = mArray[tv_id];
  return TpgBase::fault(rep->fault_id(pos));
}

// @brief 指定されたテストベクタ番号で検出された故障の出力ごとの故障伝搬状態を返す．
DiffBits
FsimResults::diffbits(
  SizeType tv_id,
  SizeType pos
) const
{
  _check_tv_id(tv_id);
  auto rep = mArray[tv_id];
  return rep->diffbits(pos);
}

END_NAMESPACE_DRUID
