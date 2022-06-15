#ifndef TPGDFFCLEAR_H
#define TPGDFFCLEAR_H

/// @file TpgDffClear.h
/// @brief TpgDffClear のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDffControl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffClear TpgDffClear.h "TpgDffClear.h"
/// @brief DFFのクリア端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffClear :
  public TpgDffControl
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgDffClear(
    const TpgDff* dff,   ///< [in] 接続しているDFF
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffClear() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のクリア端子に接続している力タイプの時 true を返す．
  bool
  is_dff_clear() const override;

};

END_NAMESPACE_DRUID

#endif // TPGDFFCLEAR_H
