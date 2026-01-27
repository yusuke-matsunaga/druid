
/// @file GateTypeMgr.cc
/// @brief GateTypeMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateTypeMgr.h"
#include "GateType_PPI.h"
#include "GateType_PPO.h"
#include "GateType_Primitive.h"
#include "GateType_Cplx.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス GateTypeMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
GateTypeMgr::GateTypeMgr()
{
  auto ppi_type = new GateType_PPI();
  mPPIType = std::unique_ptr<GateType>{ppi_type};
  auto ppo_type = new GateType_PPO();
  mPPOType = std::unique_ptr<GateType>{ppo_type};
}

// @brief デストラクタ
GateTypeMgr::~GateTypeMgr()
{
}

// @brief PPI 型を返す．
const GateType*
GateTypeMgr::ppi_type() const
{
  return mPPIType.get();
}

// @brief PPO 型を返す．
const GateType*
GateTypeMgr::ppo_type() const
{
  return mPPOType.get();
}

// @brief GateType を取り出す．
const GateType*
GateTypeMgr::gate_type(
  SizeType id
) const
{
  return mTypeDict.at(id).get();
}

// @brief 組み込み型を登録する．
void
GateTypeMgr::new_primitive(
  SizeType id,
  SizeType input_num,
  PrimType prim_type
)
{
  auto gate_type = new GateType_Primitive(input_num, prim_type);
  mTypeDict.emplace(id, std::unique_ptr<GateType>{gate_type});
}

// @brief 論理式型を登録する．
void
GateTypeMgr::new_expr(
  SizeType id,
  SizeType input_num,
  const Expr& expr
)
{
  auto prim_type = expr.analyze();
  if ( prim_type != PrimType::None ) {
    new_primitive(id, input_num, prim_type);
  }
  else {
    auto gate_type = new GateType_Cplx(input_num, expr);
    mTypeDict.emplace(id, std::unique_ptr<GateType>{gate_type});
  }
}

END_NAMESPACE_DRUID
