#ifndef DOPDUMMY_H
#define DOPDUMMY_H

/// @file DopDummy.h
/// @brief DopDummy のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DetectOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DopDummy DopDummy.h "DopDummy.h"
/// @brief DetectOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class DopDummy :
  public DetectOp
{
public:

  /// @brief コンストラクタ
  DopDummy();

  /// @brief デストラクタ
  ~DopDummy();


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

};

END_NAMESPACE_DRUID

#endif // DOPDUMMY_H
