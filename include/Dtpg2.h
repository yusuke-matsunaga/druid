#ifndef DTPG2_H
#define DTPG2_H

/// @file Dtpg2.h
/// @brief Dtpg2 のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"

#include "DtpgStats.h"
#include "FaultStatus.h"
#include "ym/SatBool3.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Dtpg2 Dtpg2.h "Dtpg2.h"
/// @brief 遷移回数を考慮した DTPG の基本エンジン
//////////////////////////////////////////////////////////////////////
class Dtpg2
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  Dtpg2(const string& sat_type,
	const string& sat_option,
	ostream* sat_outp,
	BackTracer& bt);

  /// @brief デストラクタ
  ~Dtpg2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  /// @param[in] tvmgr テストベクタのマネージャ
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] fsim 故障シミュレーター
  /// @param[in] network 対象のネットワーク
  /// @param[out] tv_list 生成されたテストパタンのリスト
  /// @param[inout] stats DTPGの統計情報
  void
  run(TvMgr& tvmgr,
      TpgFaultMgr& fmgr,
      Fsim& fsim,
      const TpgNetwork& network,
      bool use_xorsampling,
      double wsa_ratio,
      int scount_limit,
      vector<const TestVector*>& tv_list,
      DtpgStats& stats);

  /// @brief テスト生成を行なう．
  /// @param[in] tvmgr テストベクタのマネージャ
  /// @param[in] fsim 故障シミュレーター
  /// @param[in] network 対象のネットワーク
  /// @param[in] fault 対象の故障
  /// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  ///
  /// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
  /// どちらの関数も呼んでいなければなにもしないで kB3X を返す．
  SatBool3
  dtpg(TvMgr& tvmgr,
       Fsim& fsim,
       const TpgNetwork& network,
       const TpgFault* fault,
       bool use_xorsampling,
       int wsa_limit,
       int scount_limit,
       vector<TestVector*>& tv_list,
       DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  void
  make_input_constraint(TvMgr& tvmgr,
			Fsim& fsim,
			int wsa_limit);

  void
  rtpg(TvMgr& tvmgr,
       TpgFaultMgr& fmgr,
       Fsim& fsim,
       int wsa_limit,
       DetectOp& dop);

  int
  optimize(TvMgr& tvmgr,
	   Fsim& fsim,
	   int wsa_limit,
	   const NodeValList& nodeval_list,
	   TestVector* tv);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATタイプ
  string mSatType;

  // SATオプション
  string mSatOption;

  // SATのログ出力
  ostream* mSatOutP;

  // バックトレーサー
  BackTracer& mBackTracer;

  // 乱数発生器
  RandGen mRandGen;

  // 総パタン数
  int mPatNum;

  // 初期パタンがしきい値を超えたパタン数
  int mExceedNum;

  // 総繰り返し数
  int mTotalCount;

  // 解の見つかった総数
  int mTotalFound;

  // XORサンプリング数
  int mTotalSampling;

  // 値が制限を超えていた層数
  int mTotalOver;

  // 結果としてしきい値を超えたパタン数
  int mFinalExceedNum;

  // optimize 用の乱数発生器
  RandGen mRandGen2;

};

END_NAMESPACE_DRUID

#endif // DTPG2_H
