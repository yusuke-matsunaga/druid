
/// @file FaultInfoMgr.cc
/// @brief FaultInfoMgr の実装ファイル
/// @author Yusuke Mnatsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FaultInfoMgr.h"
#include "FaultInfo.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "FaultAnalyzer.h"
#include "FFRFaultList.h"
#include "OpBase.h"


#define DBG_OUT cerr

BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FaultInfoMgr::FaultInfoMgr(
  const TpgNetwork& network,
  const vector<const TpgFault*>& fault_list
) : mNetwork{network},
    mFaultList{fault_list},
    mFaultInfoArray(network.max_fault_id()),
    mRootMandCondArray(network.ffr_num())
{
}

// @brief 現時点で残っている故障リストを返す．
const vector<const TpgFault*>&
FaultInfoMgr::active_fault_list() const
{
  if ( mActiveFaultList.size() != mFaultNum ) {
    vector<const TpgFault*> fault_list;
    fault_list.reserve(mFaultNum);
    for ( auto fault: mActiveFaultList ) {
      if ( fault_info(fault).is_deleted() ) {
	continue;
      }
      fault_list.push_back(fault);
    }
    std::swap(mActiveFaultList, fault_list);
  }
  return mActiveFaultList;
}

// @brief 故障情報を求める．
void
FaultInfoMgr::generate(
  const JsonValue& option
)
{
  Timer timer;
  timer.start();

  int debug = OpBase::get_debug(option);

  for ( auto fault: mFaultList ) {
    mFaultInfoArray[fault->id()].set_fault(fault);
  }

  // FFR ごとに故障検出を行う．
  FFRFaultList ffr_fault_list{mNetwork, mFaultList};
  mActiveFaultList.clear();
  SizeType nt = 0;
  for ( auto ffr: ffr_fault_list.ffr_list() ) {
    FaultAnalyzer analyzer{mNetwork, ffr, option};
    mRootMandCondArray[ffr->id()] = analyzer.root_mandatory_condition();

    // 個々の故障について処理を行う．
    for ( auto fault: ffr_fault_list.fault_list(ffr) ) {
      auto& finfo = mFaultInfoArray[fault->id()];
      if ( analyzer.run(finfo) ) {
	mActiveFaultList.push_back(fault);
	++ mFaultNum;
	if ( finfo.is_trivial() ) {
	  ++ nt;
	}
      }
    }
  }

  timer.stop();
  if ( debug > 0 ) {
    DBG_OUT << "Total faults: " << mFaultNum
	    << " (" << nt << ")" << endl
	    << "CPU time:     "
	    << (timer.get_time() / 1000.0) << endl;
  }
}

// @brief 故障を削除する．
void
FaultInfoMgr::delete_fault(
  const TpgFault* fault ///< [in] 対象の故障
)
{
  mFaultInfoArray[fault->id()].set_deleted();
  -- mFaultNum;
}

END_NAMESPACE_DRUID
