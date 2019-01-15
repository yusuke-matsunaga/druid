#ifndef RTPG_H
#define RTPG_H

/// @file Rtpg.h
/// @brief Rtpg のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Rtpg Rtpg.h "Rtpg.h"
/// @brief ランダムパタンによる故障シミュレーションでテストパタンを求めるクラス
//////////////////////////////////////////////////////////////////////
class Rtpg
{
public:

  /// @brief コンストラクタ
  /// @param[in] network 対象のネットワーク
  /// @param[in] tvmgr TvMgr
  /// @param[in] fault_type 故障の種類
  Rtpg(const TpgNetwork& network,
       TvMgr& tvmgr,
       FaultType fault_type);

  /// @brief デストラクタ
  ~Rtpg();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief 乱数生成器を初期化する．
  /// @param[in] seed 乱数の種
  void
  randgen_init(ymuint32 seed);

  /// @brief 1セット(kPvBitLen個)のパタンで故障シミュレーションを行う．
  /// @return 新たに検出された故障数を返す．
  int
  do_fsim();

  /// @brief 検出された故障のリストを返す．
  const vector<const TpgFault*>&
  det_fault_list() const;

  /// @brief 故障を検出したパタンのリストを返す．
  const vector<const TestVector*>&
  pattern_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の種類
  FaultType mFaultType;

  // 乱数発生器
  RandGen mRandGen;

  // TvMgr
  TvMgr& mTvMgr;

  // 故障シミュレータ
  Fsim* mFsim;

  // 現在のパタンを入れておくバッファ
  TestVector* tv_array[kPvBitLen];

  // 検出された故障のリスト
  vector<const TpgFault*> mDetFaultList;

  // 故障を検出したパタンのリスト
  vector<const TestVector*> mPatternList;

};

END_NAMESPACE_DRUID

#endif // RTPG_H
