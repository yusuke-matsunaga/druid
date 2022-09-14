#ifndef TPGLOGICC0_H
#define TPGLOGICC0_H

/// @file TpgLogicC0.h
/// @brief TpgLogicC0 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicC0 TpgLogicC0.h "TpgLogicC0.h"
/// @brief constant-0 を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicC0 :
  public TpgLogic
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgLogicC0(
    SizeType fanout_num ///< [in] ファンアウト数
  );

  /// @brief デストラクタ
  ~TpgLogicC0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  GateType
  gate_type() const override;

};

END_NAMESPACE_DRUID

#endif // TPGLOGICC0_H
