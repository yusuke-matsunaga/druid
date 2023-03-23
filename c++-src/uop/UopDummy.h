#ifndef UOPDUMMY_H
#define UOPDUMMY_H

/// @file UopDummy.h
/// @brief UopDummy のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "UntestOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class UopDummy UopDummy.h "UopDummy.h"
/// @brief UntestOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class UopDummy :
  public UntestOp
{
public:

  /// @brief コンストラクタ
  UopDummy();

  /// @brief デストラクタ
  ~UopDummy();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト不能故障と判定された時の処理
  void
  operator()(
    const TpgFault& f ///< [in] 故障
  );

};

END_NAMESPACE_DRUID

#endif // UOPDUMMY_H
