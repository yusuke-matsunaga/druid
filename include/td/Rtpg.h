#ifndef TD_RTPG_H
#define TD_RTPG_H

/// @file td/Rtpg.h
/// @brief Rtpg のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/td_nsdef.h"


BEGIN_NAMESPACE_DRUID_TD

//////////////////////////////////////////////////////////////////////
/// @class Rtpg Rtpg.h "td/Rtpg.h"
/// @brief RTPG を行う基底クラス
//////////////////////////////////////////////////////////////////////
class Rtpg
{
public:

  /// @brief デストラクタ
  virtual
  ~Rtpg() {}

public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 乱数生成器を初期化する．
  /// @param[in] seed 乱数の種
  virtual
  void
  init(ymuint32 seed) = 0;

  /// @brief RTPGを行なう．
  /// @param[in] fmgr 故障マネージャ
  /// @param[in] tvmgr テストベクタマネージャ
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] min_f 1回のシミュレーションで検出する故障数の下限
  /// @param[in] max_i 故障検出できないシミュレーション回数の上限
  /// @param[in] max_pat 最大のパタン数
  /// @param[in] wsa_limit WSA の制限値
  /// @param[out] det_fault_list 検出された故障のリスト
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
      RtpgStats& stats) = 0;

};


/// @brief Rtpg のインスタンスを生成する．
extern
Rtpg*
new_RtpgP1();

/// @brief Rtpg のインスタンスを生成する．
extern
Rtpg*
new_RtpgP2(ymuint nbits = 1);

END_NAMESPACE_DRUID_TD

#endif // TD_RTPG_H
