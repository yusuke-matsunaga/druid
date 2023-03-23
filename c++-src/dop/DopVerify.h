#ifndef DOPVERIFY_H
#define DOPVERIFY_H

/// @file DopVerify.h
/// @brief DopVerify のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopVerify DopVerify.h "DopVerify.h"
/// @brief 故障シミュレーションを行なった故障ドロップを行なうクラス
//////////////////////////////////////////////////////////////////////
class DopVerify :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  DopVerify(
    Fsim& fsim,             ///< [in] 故障シミュレータ
    DopVerifyResult& result ///< [in] 結果を格納するオブジェクト
  );

  /// @brief デストラクタ
  ~DopVerify();


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

  // 故障シミュレータ
  Fsim& mFsim;

  // 結果を格納するオブジェクト
  DopVerifyResult& mResult;

};

END_NAMESPACE_DRUID

#endif // DOPVERIFY_H
