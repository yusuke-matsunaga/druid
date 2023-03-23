#ifndef OPDETECT_H
#define OPDETECT_H

/// @file DopDrop.h
/// @brief DopDrop のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopDrop DopDrop.h "DopDrop.h"
/// @brief 故障シミュレーションを行なって故障ドロップを行なうクラス
//////////////////////////////////////////////////////////////////////
class DopDrop :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  DopDrop(
    TpgFaultMgr& fmgr, ///< [in] 故障の状態を管理するクラス
    Fsim& fsim         ///< [in] 故障シミュレータ
  );

  /// @brief デストラクタ
  ~DopDrop();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  void
  operator()(
    const TpgFault& f,   ///< [in] 故障
    const TestVector& tv ///< [in] テストベクタ
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の状態を管理するクラス
  TpgFaultMgr& mFaultMgr;

  // 故障シミュレータ
  Fsim& mFsim;

};

END_NAMESPACE_DRUID

#endif // OPDETECT_H
