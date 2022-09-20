#ifndef DETECTOP_H
#define DETECTOP_H

/// @file DetectOp.h
/// @brief DetectOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultType.h"


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
  ~DetectOp() { }


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
DetectOp*
new_DopBase(
  FaultStatusMgr& fmgr ///< [in] 故障マネージャ
);

/// @brief 'drop' タイプを生成する．
DetectOp*
new_DopDrop(
  FaultStatusMgr& fmgr, ///< [in] 故障マネージャ
  Fsim& fsim		///< [in] 故障シミュレータ
);

/// @brief 'tvlist' タイプを生成する．
DetectOp*
new_DopTvList(
  SizeType input_num,        ///< [in] 入力数
  SizeType dff_num,          ///< [in] DFF数
  FaultType fault_type,      ///< [in] 故障タイプ
  vector<TestVector>& tvlist ///< [in] テストベクタのリスト
);

/// @brief 'verify' タイプを生成する．
DetectOp*
new_DopVerify(
  Fsim& fsim,             ///< [in] 故障シミュレータ
  DopVerifyResult& result ///< [in] 結果を格納するオブジェクト
);

/// @brief 'dummy' タイプを生成する．
DetectOp*
new_DopDummy();

END_NAMESPACE_DRUID

#endif // DETECTOP_H
