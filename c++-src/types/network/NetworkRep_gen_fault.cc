
/// @File NetworkRep_gen_Fault.cc
/// @brief NetworkRep の故障を生成する関数の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NetworkRep.h"
#include "NodeRep.h"
#include "GateRep.h"
#include "MFFCRep.h"
#include "FFRRep.h"
#include "FaultRep.h"
#include "types/FaultType.h"
#include "types/Fval2.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス NetworkRep
//////////////////////////////////////////////////////////////////////

// @brief ゲートに関連した故障を作る．
void
NetworkRep::gen_gate_faults(
  const GateRep* gate,
  std::vector<SizeType>& fault_map
)
{
  if ( gate->is_ppi() ) {
    // 入力に関しては網羅故障はないので縮退故障で考える．
    gen_stem_fault(gate, fault_map);
  }
  else if ( gate->is_ppo() ) {
    // 出力
    gen_branch_fault(gate);
  }
  else {
    // ゲート
    switch ( fault_type() ) {
    case FaultType::StuckAt:
    case FaultType::TransitionDelay:
      // ステムの故障
      gen_stem_fault(gate, fault_map);
      // ブランチの故障
      gen_branch_fault(gate);
      break;

    case FaultType::GateExhaustive:
      // ゲート網羅故障
      gen_ex_fault(gate);
      break;

    default:
      throw std::logic_error{"wrong fault type"};
      break;
    }
  }
}

// @brief ステムの故障を作る．
void
NetworkRep::gen_stem_fault(
  const GateRep* gate,
  std::vector<SizeType>& fault_map
)
{
  auto ftype = fault_type();
  if ( ftype == FaultType::GateExhaustive ) {
    ftype = FaultType::StuckAt;
  }
  for ( auto fval: {Fval2::zero, Fval2::one} ) {
    auto f = reg_fault([&](SizeType fid){
      return FaultRep::new_stem_fault(fid, gate, fval, ftype);
    });
    const_cast<GateRep*>(gate)->set_stem_fault(fval, f);
    auto node = gate->output_node();
    auto b = fval == Fval2::zero ? 0 : 1;
    fault_map[node->id() * 2 + b] = f->id();
  }
}

// @brief ブランチの故障を作る．
void
NetworkRep::gen_branch_fault(
  const GateRep* gate
)
{
  SizeType ni = gate->input_num();
  for ( SizeType ipos = 0; ipos < ni; ++ ipos ) {
    for ( auto fval: {Fval2::zero, Fval2::one} ) {
      auto f = reg_fault([&](SizeType fid){
	return FaultRep::new_branch_fault(fid, gate, ipos, fval, fault_type());
      });
      const_cast<GateRep*>(gate)->set_branch_fault(ipos, fval, f);
    }
  }
}

// @brief ゲート網羅故障を作る．
void
NetworkRep::gen_ex_fault(
  const GateRep* gate
)
{
  SizeType ni = gate->input_num();
  SizeType ni_exp = 1 << ni;
  std::vector<bool> ivals(ni);
  for ( SizeType b = 0; b < ni_exp; ++ b ) {
    for ( SizeType i = 0; i < ni; ++ i ) {
      if ( b & (1 << i) ) {
	ivals[i] = true;
      }
    }
    auto f = reg_fault([&](SizeType fid){
      return FaultRep::new_ex_fault(fid, gate, ivals);
    });
    const_cast<GateRep*>(gate)->set_ex_fault(ivals, f);
  }
}

// @brief 故障を生成して登録する．
FaultRep*
NetworkRep::reg_fault(
  std::function<FaultRep*(SizeType fid)> new_fault
)
{
  SizeType fid = mFaultArray.size();
  auto fault = new_fault(fid);
  mFaultArray.push_back(std::unique_ptr<FaultRep>{fault});
  return fault;
}

// @brief 代表故障マップを作る．
void
NetworkRep::gen_rep_map(
  const GateRep* gate,
  const std::vector<SizeType>& fault_map,
  std::vector<SizeType>& rep_map
)
{
  auto gid = gate->id();

  // 出力のステムの故障
  auto f0 = gate->stem_fault(Fval2::zero);
  auto f1 = gate->stem_fault(Fval2::one);

  SizeType NI = gate->input_num();
  for ( SizeType i = 0; i < NI; ++ i ) {
    for ( SizeType v = 0; v < 2; ++ v ) {
      auto val = v == 0 ? Val3::_0 : Val3::_1;
      auto fval = v == 0 ? Fval2::zero : Fval2::one;
      auto i_fault = gate->branch_fault(i, fval);

      // 1. 入力の故障が出力の故障と等価か調べる．
      auto oval = gate->cval(i, val);
      const FaultRep* o_fault = nullptr;
      switch ( oval ) {
      case Val3::_0:
	o_fault = f0;
	break;
      case Val3::_1:
	o_fault = f1;
	break;
      case Val3::_X:
	break;
      }
      if ( o_fault != nullptr ) {
	rep_map[i_fault->id()] = o_fault->id();
      }

      // 2. 入力のファンアウト数が1の時，その入力のステムの故障と等価となる．
      auto inode = gate->input_node(i);
      if ( inode->fanout_num() == 1 ) {
	auto s_id = fault_map[inode->id() * 2 + v];
	rep_map[s_id] = i_fault->id();
      }
    }
  }
}

END_NAMESPACE_DRUID
