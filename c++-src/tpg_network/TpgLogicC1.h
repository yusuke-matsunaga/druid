#ifndef TPGLOGICC1_H
#define TPGLOGICC1_H

/// @file TpgLogicC1.h
/// @brief TpgLogicC1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogic.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicC1 TpgLogicC1.h "TpgLogicC1.h"
/// @brief constant-1 を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicC1 :
  public TpgLogic0
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgLogicC1(
    SizeType fanout_num ///< [in] ファンアウト数
  );

  /// @brief デストラクタ
  ~TpgLogicC1() = default;


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

#endif // TPGLOGICC1_H
