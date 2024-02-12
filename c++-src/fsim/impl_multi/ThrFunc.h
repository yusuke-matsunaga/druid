#ifndef THRFUNC_H
#define THRFUNC_H

/// @file ThrFunc.h
/// @brief ThrFunc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "FsimX.h"
#include "EventQ.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SyncObj;

//////////////////////////////////////////////////////////////////////
/// @class ThrFunc ThrFunc.h "ThrFunc.h"
/// @brief ppsfp タイプのイベントドリブンシミュレーションを行うスレッド用のクラス
//////////////////////////////////////////////////////////////////////
class ThrFunc
{
public:

  /// @brief コンストラクタ
  ThrFunc(
    SizeType id,          ///< [in] ID番号
    FSIM_CLASSNAME& fsim, ///< [in] 故障シミュレータ本体
    SyncObj& sync         ///< [in] 同期用のオブジェクト
  );

  /// @brief デストラクタ
  ~ThrFunc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief スレッド実行の本体
  void
  main_loop();

  /// @brief 結果を読み出す．
  const vector<pair<TpgFault, DiffBits>>&
  res_list(
    SizeType bit ///< [in] ビット位置
  ) const
  {
    return mResList[bit];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief PPSFP 法のシミュレーションを行う．
  void
  ppsfp();

  /// @brief SPPFP 法のシミュレーションを行う．
  void
  sppfp();

  /// @brief sppsp() 用の下請け関数
  void
  sppfp_simulation(
    const vector<const SimFFR*>& ffr_array ///< [in] 対象のFFRのリスト
  );

  void
  log(
    const string& msg
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // 故障シミュレータ
  FSIM_CLASSNAME& mFsim;

  // 同期用のオブジェクト
  SyncObj& mSyncObj;

  // イベントキュー
  EventQ mEventQ;

  // 一時的に結果を貯めておくバッファの排列
  vector<pair<TpgFault, DiffBits>> mResList[PV_BITLEN];

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID_FSIM

#endif // THRFUNC_H
