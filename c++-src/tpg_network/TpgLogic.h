﻿#ifndef TPGLOGIC_H
#define TPGLOGIC_H

/// @file TpgLogic.h
/// @brief TpgLogic のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgLogic TpgLogic.h "TpgLogic.h"
/// @brief 論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgLogic :
  public TpgNode
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  TpgLogic(
    const vector<const TpgNode*>& fanin_list, ///< [in] ファンインリスト
    SizeType fanout_num                       ///< [in] ファンアウト数
  );

  /// @brief デストラクタ
  ~TpgLogic() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief logic タイプの時 true を返す．
  bool
  is_logic() const override;

};

END_NAMESPACE_DRUID

#endif // TPGLOGIC_H
