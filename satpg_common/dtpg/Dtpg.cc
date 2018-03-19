﻿
/// @file Dtpg.cc
/// @brief Dtpg の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Dtpg.h"

#include "DtpgImpl_new.h"
//#include "DtpgImplM.h"

#include "TpgMFFC.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] jt 正当化を行うファンクタ
Dtpg::Dtpg(const string& sat_type,
	   const string& sat_option,
	   ostream* sat_outp,
	   FaultType fault_type,
	   Justifier& jt) :
  mSatType(sat_type),
  mSatOption(sat_option),
  mSatOutP(sat_outp),
  mFaultType(fault_type),
  mJustifier(jt),
  mImpl(nullptr)
{
}

// @brief デストラクタ
Dtpg::~Dtpg()
{
  delete mImpl;
}

// @brief 回路の構造を表すCNF式を作る(FFRモード)．
// @param[in] network 対象のネットワーク
// @param[in] ffr 故障伝搬の起点となる FFR
// @param[out] stats DTPGの統計情報
void
Dtpg::gen_ffr_cnf(const TpgNetwork& network,
		  const TpgFFR* ffr,
		  DtpgStats& stats)
{
  if ( mImpl != nullptr ) {
    // 以前の実装オブジェクトは破棄する．
    delete mImpl;
  }

  mImpl = new nsDtpg::DtpgImpl(mSatType, mSatOption, mSatOutP, mFaultType,
			       mJustifier, network.node_num());
  mImpl->gen_cnf(ffr->root(), stats);
}

// @brief 回路の構造を表すCNF式を作る(MfFCモード)．
// @param[in] network 対象のネットワーク
// @param[in] mffc 故障伝搬の起点となる MFFC
// @param[out] stats DTPGの統計情報
//
// この MFFC に含まれるすべての FFR が対象となる．
// FFR と MFFC が一致している場合は gen_ffr_cnf と同じことになる．
void
Dtpg::gen_mffc_cnf(const TpgNetwork& network,
		   const TpgMFFC* mffc,
		   DtpgStats& stats)
{
  if ( mImpl != nullptr ) {
    // 以前の実装オブジェクトは破棄する．
    delete mImpl;
  }

  mImpl = new nsDtpg::DtpgImpl(mSatType, mSatOption, mSatOutP, mFaultType,
			       mJustifier, network.node_num());
  mImpl->gen_cnf(mffc, stats);
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
//
// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
// どちらの関数も呼んでいなければなにもしないで kB3X を返す．
SatBool3
Dtpg::dtpg(const TpgFault* fault,
	   NodeValList& nodeval_list,
	   DtpgStats& stats)
{
  if ( mImpl == nullptr ) {
    cerr << "Error[Dtpg::dtpg()]: Neither gen_ffr_cnf() nor gen_mffc_cnf() have not been called."
	 << endl;
    return kB3X;
  }

  SatBool3 ans = mImpl->dtpg(fault, nodeval_list, stats);

  return ans;
}

END_NAMESPACE_YM_SATPG
