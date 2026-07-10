
/// @file PatGen.cc
/// @brief PatGen の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "PatGen.h"
#include "PGImpl_Random.h"
#include "PGImpl_TV.h"
#include "fsim/FsimResults.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス PatGen
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
PatGen::PatGen(
  const FaultInfo& fault_info,
  const ConfigParam& option
)
{
  auto impl1 = PGImpl_TV(fault_info);
  auto impl2 = PGImpl_Random(fault_info);
  mImplList.push_back(std::unique_ptr<PGImpl>{impl1});
  mImplList.push_back(std::unique_ptr<PGImpl>{impl2});
}

// @brief デストラクタ
PatGen::~PatGen()
{
}

// @brief パタンを作る．
void
PatGen::operator()(
  SizeType size,
  std::vector<TestVector>& tv_buff
)
{
  for ( SizeType i = 0; i < size; ++ i ) {
    for ( auto& impl: mImplList ) {
      if ( impl->get_pat(tv_buff[i]) ) {
	break;
      }
    }
  }
}

// @brief 検出結果で更新する．
void
PatGen::update(
  const FsimResults& res
)
{
  for ( auto& impl: mImplList ) {
    impl->update(res);
  }
}

END_NAMESPACE_DRUID
