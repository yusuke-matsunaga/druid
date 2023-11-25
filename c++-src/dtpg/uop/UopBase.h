#ifndef UOPBASE_H
#define UOPBASE_H

/// @file UopBase.h
/// @brief UopBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "UntestOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class UopBase UopBase.h "UopBase.h"
/// @brief UntestOp の基本的な動作を行なうクラス
//////////////////////////////////////////////////////////////////////
class UopBase :
  public UntestOp
{
public:

  /// @brief コンストラクタ
  UopBase(
    TpgFaultMgr& fmgr ///< [in] 故障マネージャ
  );

  /// @brief デストラクタ
  ~UopBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト不能故障と判定された時の処理
  void
  operator()(
    const TpgFault& f ///< [in] 故障
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障マネージャ
  TpgFaultMgr& mMgr;

};

END_NAMESPACE_DRUID

#endif // UOPBASE_H
