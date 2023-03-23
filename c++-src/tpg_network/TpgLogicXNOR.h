#ifndef TPGLOGICXNOR_H
#define TPGLOGICXNOR_H

/// @file TpgLogicXNOR.h
/// @brief TpgLogicXNOR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgLogicXOR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicXNOR2 TpgLogicXNOR.h "TpgLogicXNOR.h"
/// @brief XNORを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogicXNOR :
  public TpgLogicXOR
{
public:

  /// @brief コンストラクタ
  TpgLogicXNOR(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  ~TpgLogicXNOR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

};

END_NAMESPACE_DRUID

#endif // TPGLOGICXNOR_H
