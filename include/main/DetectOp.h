#ifndef DETECTOP_H
#define DETECTOP_H

/// @file DetectOp.h
/// @brief DetectOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DetectOp DetectOp.h "DetectOp.h"
/// @brief テストパタンが見つかったときの処理を行なうファンクタ
//////////////////////////////////////////////////////////////////////
class DetectOp
{
public:

  /// @brief デストラクタ
  virtual
  ~DetectOp() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  virtual
  void
  operator()(
    const TpgFault* f,   ///< [in] 故障
    const TestVector& tv ///< [in] テストベクタ
  ) = 0;

};

/// @brief 'base' タイプを生成する．
///
/// 故障マネージャの状態を変更する．
DetectOp*
new_DopBase(
  FaultStatusMgr& fmgr ///< [in] 故障マネージャ
);

/// @brief 'drop' タイプを生成する．
///
/// 求まったパタンで故障シミュレーションを行い
/// 検出できる故障を求める．
DetectOp*
new_DopDrop(
  FaultStatusMgr& fmgr, ///< [in] 故障マネージャ
  Fsim& fsim		///< [in] 故障シミュレータ
);

/// @brief 'tvlist' タイプを生成する．
///
/// 求まったパタンを tvlist に加える．
DetectOp*
new_DopTvList(
  vector<TestVector>& tvlist ///< [in] テストベクタのリスト
);

/// @brief 'verify' タイプを生成する．
///
/// 故障シミュレータを用いてパタンの検証を行う．
/// 結果は result に蓄積される．
DetectOp*
new_DopVerify(
  Fsim& fsim,             ///< [in] 故障シミュレータ
  DopVerifyResult& result ///< [in] 結果を格納するオブジェクト
);

/// @brief 'dummy' タイプを生成する．
///
/// なにもシない．
DetectOp*
new_DopDummy();

END_NAMESPACE_DRUID

#endif // DETECTOP_H
