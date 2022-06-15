
/// @File AuxNodeInfo.cc
/// @brief AuxNodeInfo の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019 Yusuke Matsunaga
/// All rights reserved.

#include "AuxNodeInfo.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス AuxNodeInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AuxNodeInfo::AuxNodeInfo(
  const string& name,
  int ni
)
{
  init(name, ni);
}

// @brief デストラクタ
AuxNodeInfo::~AuxNodeInfo()
{
}

// @brief 初期化する．
void
AuxNodeInfo::init(
  const string& name,
  int ni
)
{
  mName = name;
  mFaninNum = ni;

  int ni2 = ni * 2;
  mInputFaults.clear();
  mInputFaults.resize(ni2, nullptr);
}

// @brief このノードが持っている代表故障をリストに追加する．
void
AuxNodeInfo::add_to_fault_list(vector<const TpgFault*>& fault_list)
{
  for ( int i: Range(0, mFaultNum) ) {
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
