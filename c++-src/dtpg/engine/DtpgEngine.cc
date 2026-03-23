
/// @file DtpgEngine.cc
/// @brief DtpgEngine の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "dtpg/DtpgEngine.h"
#include "FaultEnc_Node.h"
#include "FaultEnc_FFR.h"
#include "FaultEnc_MFFC.h"
#include "MFFCEnc.h"
#include "types/TpgFault.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ(Nodeモード)
DtpgEngine::DtpgEngine(
  const TpgNode& node,
  const ConfigParam& option
) : mEngine(node, option),
    mFaultEnc{new FaultEnc_Node}
{
}

// @brief コンストラクタ(FFRモード)
DtpgEngine::DtpgEngine(
  const TpgFFR& ffr,
  const ConfigParam& option
) : mEngine(ffr.root(), option),
    mFaultEnc{new FaultEnc_FFR}
{
}

// @brief コンストラクタ(MFFCモード)
DtpgEngine::DtpgEngine(
  const TpgMFFC& mffc,
  const ConfigParam& option
) : mEngine(mffc.root(), option),
    mFaultEnc{new FaultEnc_MFFC(mEngine, mffc)}
{
}

// @brief デストラクタ
DtpgEngine::~DtpgEngine()
{
}

// @brief 故障を検出する条件を返す．
std::vector<SatLiteral>
DtpgEngine::make_detect_condition(
  const TpgFault& fault
)
{
  auto cond = mFaultEnc->fault_propagate_condition(fault);
  auto lits = mEngine.conv_to_literal_list(cond);
  lits.push_back(mEngine.prop_var());
  mFaultEnc->add_aux_condition(fault, lits);
  return lits;
}

// @brief SATの解から十分条件を得る．
SuffCond
DtpgEngine::extract_sufficient_condition(
  const TpgFault& fault,
  const SatModel& model
)
{
  auto cond = mEngine.extract_sufficient_condition(model);
  return mFaultEnc->extract_sufficient_condition(mEngine, fault, cond, model);
}

// @brief SATの解から十分条件を得る．
SuffCond
DtpgEngine::extract_sufficient_condition(
  const TpgFault& fault,
  SizeType pos,
  const SatModel& model
)
{
  auto cond = mEngine.extract_sufficient_condition(pos, model);
  return mFaultEnc->extract_sufficient_condition(mEngine, fault, cond, model);
}

// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
AssignList
DtpgEngine::justify(
  const SuffCond& suff_cond,
  const SatModel& model
)
{
  return mEngine.justify(suff_cond, model);
}

// @brief 値割り当てを対応するリテラルに変換する．
SatLiteral
DtpgEngine::conv_to_literal(
  Assign assign
)
{
  return mEngine.conv_to_literal(assign);
}

// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
std::vector<SatLiteral>
DtpgEngine::conv_to_literal_list(
  const AssignList& assign_list
)
{
  return mEngine.conv_to_literal_list(assign_list);
}

// @brief 現在の内部状態を得る．
SatStats
DtpgEngine::get_stats() const
{
  return mEngine.get_stats();
}

// @brief CNF の生成時間を返す．
double
DtpgEngine::cnf_time() const
{
  return mEngine.cnf_time();
}

END_NAMESPACE_DRUID
