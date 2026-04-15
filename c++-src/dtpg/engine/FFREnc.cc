
/// @file FFREnc.cc
/// @brief FFREnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "FFREnc.h"
#include "types/TpgNetwork.h"


BEGIN_NAMESPACE_DRUID

// @brief コンストラクタ
FFREnc::FFREnc(
  const TpgFFR& ffr,
  const TpgFaultList& fault_list,
  const ConfigParam& option
) : BoolDiffEnc(ffr.root(), option),
    mFFR{ffr},
    mFaultList{fault_list},
    mDLitsArray(mFaultList.size()),
    mULitArray(mFaultList.size()),
    mHasULit{option.get_bool_elem("has_ulit", false)}
{
  SizeType i = 0;
  for ( auto fault: mFaultList ) {
    mFaultMap.emplace(fault.id(), i);
    ++ i;
  }
}

// @brief デストラクタ
FFREnc::~FFREnc()
{
}

// @brief 条件の生成を行う．
void
FFREnc::make_cond()
{
  auto n = mFaultList.size();
  auto pvar = prop_var();
  for ( SizeType i = 0; i < n; ++ i ) {
    auto fault = mFaultList[i];
    auto cond = fault.ffr_propagate_condition();
    auto dlits = conv_to_literal_list(cond);
    dlits.push_back(pvar);
    mDLitsArray[i] = dlits;
    if ( mHasULit ) {
      auto ulit = solver().new_variable(true);
      mULitArray[i] = ulit;
      std::vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(dlits.size() + 1);
      tmp_lits.push_back(~ulit);
      for ( auto lit: dlits ) {
	tmp_lits.push_back(~lit);
      }
      solver().add_clause(tmp_lits);
    }
  }
}

// @brief 故障の検出条件を表すリテラルのリストを返す．
const std::vector<SatLiteral>&
FFREnc::dlits(
  const TpgFault& fault
) const
{
  return mDLitsArray[local_id(fault)];
}

// @brief 故障の非検出条件を表すリテラルを返す．
SatLiteral
FFREnc::ulit(
  const TpgFault& fault
) const
{
  if ( !mHasULit ) {
    throw std::logic_error{"\"has_ulit\" is not specified"};
  }
  return mULitArray[local_id(fault)];
}

// @brief 名前を返す．
std::string
FFREnc::name()
{
  std::ostringstream buf;
  buf << "FFREnc(FFR#" << mFFR.id() << ")";
  return buf.str();
}

END_NAMESPACE_DRUID
