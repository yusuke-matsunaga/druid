#ifndef TPGDFFCLOCK_H
#define TPGDFFCLOCK_H

/// @file TpgDffClock.h
/// @brief TpgDffClock のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDffControl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffClock TpgDffClock.h "TpgDffClock.h"
/// @brief DFFのクロックを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffClock :
  public TpgDffControl
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgDffClock(
    const TpgDff* dff,   /// @param[in] 接続しているDFF
    const TpgNode* fanin /// @param[in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffClock() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
  bool
  is_dff_clock() const override;

};

END_NAMESPACE_DRUID

#endif // TPGDFFCLOCK_H
