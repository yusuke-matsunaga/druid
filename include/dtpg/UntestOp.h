#ifndef UNTESTOP_H
#define UNTESTOP_H

/// @file UntestOp.h
/// @brief UntestOp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class UntestOp UntestOp.h "UntestOp.h"
/// @brief テスト不能故障と判定された時の処理を行なうファンクタ
//////////////////////////////////////////////////////////////////////
class UntestOp
{
public:

  /// @brief デストラクタ
  virtual
  ~UntestOp() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト不能故障と判定された時の処理
  virtual
  void
  operator()(
    const TpgFault& f ///< [in] 故障
  ) = 0;

};

/// @brief 'base' タイプを生成する．
///
/// fmgr に登録する．
UntestOp*
new_UopBase(
  TpgFaultMgr& fmgr ///< [in] 故障マネージャ
);

/// @brief 'skip' タイプを生成する．
UntestOp*
new_UopSkip(
  SizeType threshold ///< [in] しきい値
);

/// @brief 'dummy' タイプを生成する．
///
/// なにもしない．
UntestOp*
new_UopDummy();

END_NAMESPACE_DRUID

#endif // UNTESTOP_H
