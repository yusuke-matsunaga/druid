﻿#ifndef EQCHECKER_H
#define EQCHECKER_H

/// @file EqChecker.h
/// @brief EqChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "sa/sa_nsdef.h"
#include "EqSet.h"
#include "ym/RandGen.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG_SA

class FaultAnalyzer;

//////////////////////////////////////////////////////////////////////
/// @class EqChecker EqChecker.h "EqChecker.h"
/// @brief 故障間の支配関係を解析するクラス
//////////////////////////////////////////////////////////////////////
class EqChecker
{
public:

  /// @brief コンストラクタ
  /// @param[in] analyzer 故障の情報を持つクラス
  /// @param[in] tvmgr テストベクタのマネージャ
  /// @param[in] fsim 故障シミュレータ
  EqChecker(FaultAnalyzer& analyzer,
	    TvMgr& tvmgr,
	    Fsim& fsim);

  /// @brief デストラクタ
  virtual
  ~EqChecker();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief verbose フラグを設定する．
  /// @param[in] verbose 表示を制御するフラグ
  void
  set_verbose(int verbose);

  /// @brief 代表故障を求める．
  /// @param[in] src_fid_list 故障リスト
  /// @param[out] rep_fid_list 結果の代表故障を格納するスト
  void
  get_rep_faults(const vector<ymuint>& src_fid_list,
		 vector<ymuint>& rep_fid_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションを行い，等価故障の候補リストを作る．
  /// @param[in] fid_list 故障リスト
  ///
  /// 結果は mEqSet に格納される．
  void
  do_fsim(const vector<ymuint>& fid_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 表示を制御するフラグ
  int mVerbose;

  // 故障の情報を持つクラス
  FaultAnalyzer& mAnalyzer;

  // テストベクタを管理するクラス
  TvMgr& mTvMgr;

  // 故障シミュレータ
  Fsim& mFsim;

  // 最大故障番号
  ymuint mMaxFaultId;

  // テストベクタ用の乱数生成器
  RandGen mRandGen;

  // 等価故障の候補リストを表すクラス
  EqSet mEqSet;

  // 故障シミュレーションで用いられたパタン数
  ymuint mPat;

};

END_NAMESPACE_YM_SATPG_SA

#endif // EQCHECKER_H
