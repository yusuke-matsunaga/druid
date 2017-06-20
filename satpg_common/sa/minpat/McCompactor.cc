
/// @file McCompactor.cc
/// @brief McCompactor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "McCompactor.h"
#include "Fsim.h"
#include "TpgFault.h"
#include "ym/MinCov.h"


BEGIN_NAMESPACE_YM_SATPG_SA

//////////////////////////////////////////////////////////////////////
// クラス McCompactor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsim 故障シミュレータ
McCompactor::McCompactor(Fsim& fsim) :
  mFsim(fsim)
{
  mVerbose = 0;
}

// @brief デストラクタ
McCompactor::~McCompactor()
{
}

// @brief verbose フラグを設定する．
// @param[in] verbose 指定する値
void
McCompactor::set_verbose(ymuint verbose)
{
  mVerbose = verbose;
}

// @brief 故障グループを圧縮する．
// @param[in] fault_list 故障リスト
// @param[in] orig_tv_list もとのテストセット
// @param[out] tv_list テストセット
void
McCompactor::run(const vector<const TpgFault*>& fault_list,
		 const vector<const TestVector*>& orig_tv_list,
		 vector<const TestVector*>& tv_list)
{
  // 故障番号の最大値を求める．
  ymuint max_fault_id = 0;
  ymuint nf = fault_list.size();
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = fault_list[i];
    ymuint fid = fault->id();
    if ( max_fault_id < fid ) {
      max_fault_id = fid;
    }
  }

  // 故障番号から fault_list 中の位置を求める表
  vector<ymuint> row_map(max_fault_id, 0);
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = fault_list[i];
    ymuint fid = fault->id();
    row_map[fid] = i;
  }

  mFsim.clear_skip(fault_list);

  ymuint np = orig_tv_list.size();
  MinCov mincov;
  mincov.set_size(nf, np);
  for (ymuint i = 0; i < np; ++ i) {
    const TestVector* tv = orig_tv_list[i];
    ymuint n = mFsim.sa_sppfp(tv);
    for (ymuint j = 0; j < n; ++ j) {
      const TpgFault* f = mFsim.det_fault(j);
      ymuint row_pos = row_map[f->id()];
      if ( row_pos > 0 ) {
	-- row_pos;
	mincov.insert_elem(row_pos, i);
      }
    }
  }

  // 最終被覆問題を解く
  vector<ymuint32> solution;
  mincov.exact(solution);

  // solution の内容から tv_list を作る．
  ymuint n = solution.size();
  tv_list.clear();
  tv_list.reserve(n);
  for (ymuint i = 0; i < n; ++ i) {
    ymuint j = solution[i];
    tv_list.push_back(orig_tv_list[j]);
  }
}

END_NAMESPACE_YM_SATPG_SA
