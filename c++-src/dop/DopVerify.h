#ifndef DOPVERIFY_H
#define DOPVERIFY_H

/// @file DopVerify.h
/// @brief DopVerify のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
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
  /// @param[in] fsim 故障シミュレータ
  /// @param[in] result 結果を格納するオブジェクト
  DopVerify(Fsim& fsim,
	    DopVerifyResult& result);

  /// @brief デストラクタ
  virtual
  ~DopVerify();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  /// @param[in] f 故障
  /// @param[in] tv テストベクタ
  void
  operator()(const TpgFault* f,
	     const TestVector& tv) override;


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
