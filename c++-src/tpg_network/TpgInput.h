#ifndef TPGINPUT_H
#define TPGINPUT_H

/// @file TpgInput.h
/// @brief TpgInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPI.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgInput TpgInput.h "TpgInput.h"
/// @brief 入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgInput :
  public TpgPPI
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgInput(
    SizeType input_id,       ///< [in] 入力番号
    SizeType fanout_num ///< [in] ファンアウト数
  );

  /// @brief デストラクタ
  ~TpgInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力タイプの時 true を返す．
  bool
  is_primary_input() const override;

};

END_NAMESPACE_DRUID

#endif // TPGINPUT_H
