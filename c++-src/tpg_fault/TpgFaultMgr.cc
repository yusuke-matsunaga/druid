
/// @file TpgFaultMgr.cc
/// @brief TpgFaultMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFaultMgr.h"
#include "TpgFaultMgrImpl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgFaultMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgFaultMgr::TpgFaultMgr()
{
}

// @brief デストラクタ
TpgFaultMgr::~TpgFaultMgr()
{
}

// @brief 故障のリストを作る．
void
TpgFaultMgr::gen_fault_list(
  const TpgNetwork& network,
  FaultType fault_type,
  const string& red_mode
)
{
  mImpl = TpgFaultMgrImpl::new_obj(network, fault_type, red_mode);
}

// @brief 故障番号から故障を得る．
TpgFault
TpgFaultMgr::fault(
  SizeType id
) const
{
  return TpgFault{mImpl.get(), id};
}

// @brief 故障のタイプを返す．
FaultType
TpgFaultMgr::fault_type() const
{
  ASSERT_COND( mImpl.get() != nullptr );

  return mImpl->fault_type();
}

// @brief 全故障のリストを返す．
TpgFaultList
TpgFaultMgr::fault_list() const
{
  ASSERT_COND( mImpl.get() != nullptr );

  // なんかまだるっこしいことをやってる．
  SizeType nf = mImpl->fault_num();
  vector<SizeType> tmp_list(nf);
  for ( SizeType i = 0; i < nf; ++ i ) {
    tmp_list[i] = i;
  }
  return TpgFaultList{mImpl.get(), tmp_list};
}

// @brief 全代表故障のリストを返す．
TpgFaultList
TpgFaultMgr::rep_fault_list() const
{
  ASSERT_COND( mImpl.get() != nullptr );

  return TpgFaultList{mImpl.get(), mImpl->rep_fault_list()};
}

// @brief ノードに関係する代表故障のリストを返す．
TpgFaultList
TpgFaultMgr::node_fault_list(
  SizeType node_id
) const
{
  ASSERT_COND( mImpl.get() != nullptr );

  return TpgFaultList{mImpl.get(), mImpl->node_fault_list(node_id)};
}

// @brief FFR に関係する代表故障のリストを返す．
TpgFaultList
TpgFaultMgr::ffr_fault_list(
  SizeType ffr_id
) const
{
  ASSERT_COND( mImpl.get() != nullptr );

  return TpgFaultList{mImpl.get(), mImpl->ffr_fault_list(ffr_id)};
}

// @brief MFFC に関係する代表故障のリストを返す．
TpgFaultList
TpgFaultMgr::mffc_fault_list(
  SizeType mffc_id
) const
{
  ASSERT_COND( mImpl.get() != nullptr );

  return TpgFaultList{mImpl.get(), mImpl->mffc_fault_list(mffc_id)};
}

// @brief 故障の状態をセットする．
void
TpgFaultMgr::set_status(
  const TpgFault& fault,
  FaultStatus status
)
{
  ASSERT_COND( mImpl.get() != nullptr );

  mImpl->set(fault.id(), status);
}

// @brief 故障の状態を得る．
FaultStatus
TpgFaultMgr::get_status(
  const TpgFault& fault
) const
{
  ASSERT_COND( mImpl.get() != nullptr );

  return mImpl->get(fault.id());
}

END_NAMESPACE_DRUID
