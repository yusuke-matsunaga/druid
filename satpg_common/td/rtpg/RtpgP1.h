﻿#ifndef RTPGP1_H
#define RTPGP1_H

/// @file RtpgP1.h
/// @brief RtpgP1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/Rtpg.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG_TD

//////////////////////////////////////////////////////////////////////
/// @class RtpgP1 RtpgP1.h "RtpgP1.h"
/// @brief Fsim を使う Rtpg
//////////////////////////////////////////////////////////////////////
class RtpgP1 :
  public Rtpg
{
public:

  /// @brief コンストラクタ
  RtpgP1();

  /// @brief デストラクタ
  virtual
  ~RtpgP1();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 乱数生成器を初期化する．
  /// @param[in] seed 乱数の種
  virtual
  void
  init(ymuint32 seed);

  /// @brief RTPGを行なう．
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] tvmgr テストベクタマネージャ
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] min_f 1回のシミュレーションで検出する故障数の下限
  /// @param[in] max_i 故障検出できないシミュレーション回数の上限
  /// @param[in] max_pat 最大のパタン数
  /// @param[in] wsa_limit WSA の制限値
  /// @param[out] tvlist テストベクタのリスト
  /// @param[out] stats 実行結果の情報を格納する変数
  virtual
  void
  run(TpgFaultMgr& fmgr,
      TvMgr& tvmgr,
      Fsim& fsim,
      ymuint min_f,
      ymuint max_i,
      ymuint max_pat,
      ymuint wsa_limit,
      vector<const TestVector*>& tvlist,
      RtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 乱数生成器
  RandGen mRandGen;

  // 制限値
  ymuint mLimit;

};

END_NAMESPACE_YM_SATPG_TD

#endif // RTPGP1_H
