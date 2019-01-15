
/// @File AuxNodeInfo.cc
/// @brief AuxNodeInfo の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "AuxNodeInfo.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス AuxNodeInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AuxNodeInfo::AuxNodeInfo()
{
  mName = nullptr;
  mFfr = nullptr;
  mMffc = nullptr;
  mFaultNum = 0;
  mFaultList = nullptr;
  mOutputFaults[0] = nullptr;
  mOutputFaults[1] = nullptr;
  mFaninNum = 0;
  mInputFaults = nullptr;
}

// @brief デストラクタ
AuxNodeInfo::~AuxNodeInfo()
{
  // このクラスに関係するメモリはすべて
  // TpgNetwork::mAlloc が管理しているので
  // ここではなにもする必要はない．
}

// @brief 初期化する．
// @param[in] name 名前
// @param[in] ni 入力数
// @param[in] alloc メモリアロケータ
void
AuxNodeInfo::init(const string& name,
		  int ni,
		  Alloc& alloc)
{
  int n = name.size();
  mName = alloc.get_array<char>(n + 1);
  for ( int i = 0; i < n; ++ i ) {
    mName[i] = name[i];
  }
  mName[n] = '\0';

  mFaninNum = ni;

  int ni2 = ni * 2;
  mInputFaults = alloc.get_array<TpgFaultBase*>(ni2);
  for ( int i = 0; i < ni2; ++ i ) {
    mInputFaults[i] = nullptr;
  }
}

// @brief FFR を設定する．
// @param[in] ffr このノードを根とするFFR
void
AuxNodeInfo::set_ffr(TpgFFR* ffr)
{
  mFfr = ffr;
}

// @brief MFFC を設定する．
// @param[in] mffc このノードを根とするMFFC
void
AuxNodeInfo::set_mffc(const TpgMFFC* mffc)
{
  mMffc = mffc;
}

// @brief 故障リストを設定する．
void
AuxNodeInfo::set_fault_list(int fault_num,
			    const TpgFault** fault_list)
{
  mFaultNum = fault_num;
  mFaultList = fault_list;
}

// @brief このノードが持っている代表故障をリストに追加する．
void
AuxNodeInfo::add_to_fault_list(vector<const TpgFault*>& fault_list)
{
  for ( int i = 0; i < mFaultNum; ++ i ) {
    fault_list.push_back(mFaultList[i]);
  }
}

// @brief 出力の故障を設定する．
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] f 故障
void
AuxNodeInfo::set_output_fault(int val,
			      TpgFaultBase* f)
{
  ASSERT_COND( val == 0 || val == 1 );

  mOutputFaults[val] = f;
}

// @brief 入力の故障を設定する．
// @param[in] ipos 入力位置
// @param[in] val 故障値 ( 0 / 1 )
// @param[in] f 故障
void
AuxNodeInfo::set_input_fault(int ipos,
			     int val,
			     TpgFaultBase* f)
{
  ASSERT_COND( val == 0 || val == 1 );
  ASSERT_COND( ipos >= 0 && ipos < mFaninNum );

  mInputFaults[(ipos * 2) + val] = f;
}

END_NAMESPACE_DRUID
