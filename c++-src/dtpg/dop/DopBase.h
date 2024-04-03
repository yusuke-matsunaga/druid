#ifndef DOPBASE_H
#define DOPBASE_H

/// @file DopBase.h
/// @brief DopBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopBase DopBase.h "DopBase.h"
/// @brief DetectOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class DopBase :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  DopBase(
    DtpgMgr& fmgr ///< [in] DTPGマネージャ
  );

  /// @brief デストラクタ
  ~DopBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが見つかった時の処理
  void
  operator()(
    const TpgFault* f,   ///< [in] 故障
    const TestVector& tv ///< [in] テストベクタ
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // DTPGマネージャ
  DtpgMgr& mMgr;

};

END_NAMESPACE_DRUID

#endif // DOPBASE_H
