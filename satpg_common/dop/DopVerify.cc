﻿
/// @file DopVerify.cc
/// @brief DopVerify の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopVerify.h"
#include "DopVerifyResult.h"
#include "TpgFault.h"
#include "Fsim.h"
#include "NodeValList.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'verify' タイプを生成する．
// @param[in] fsim 故障シミュレータ
// @param[in] result 結果を格納するオブジェクト
DetectOp*
new_DopVerify(Fsim& fsim,
	      DopVerifyResult& result)
{
  return new DopVerify(fsim, result);
}


//////////////////////////////////////////////////////////////////////
// クラス DopVerify
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsim 故障シミュレータ
// @param[in] result 結果を格納するオブジェクト
DopVerify::DopVerify(Fsim& fsim,
		     DopVerifyResult& result) :
  mFsim(fsim),
  mResult(result)
{
}

// @brief デストラクタ
DopVerify::~DopVerify()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopVerify::operator()(const TpgFault* f,
		      const NodeValList& assign_list)
{
  bool detect = mFsim.spsfp(assign_list, f);
  if ( detect ) {
    mResult.add_good(f);
  }
  else {
    mResult.add_error(f, assign_list);
  }
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] tv テストベクタ
void
DopVerify::operator()(const TpgFault* f,
		      const TestVector* tv)
{
  bool detect = mFsim.spsfp(tv, f);
  if ( detect ) {
    mResult.add_good(f);
  }
  else {
    //mResult.add_error(f, assign_list);
  }
}

END_NAMESPACE_YM_SATPG
