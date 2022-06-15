#ifndef TPGDFFCONTROL_H
#define TPGDFFCONTROL_H

/// @file TpgDffControl.h
/// @brief TpgDffControl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffControl TpgDffControl.h "TpgDffControl.h"
/// @brief DFFの制御端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffControl :
  public TpgNode
{
protected:

  /// @brief コンストラクタ
  TpgDffControl(
    const TpgDff* dff,   ///< [in] 接続しているDFF
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  virtual
  ~TpgDffControl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  const TpgDff*
  dff() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF
  const TpgDff* mDff;

};

END_NAMESPACE_DRUID

#endif // TPGDFFCONTROL_H
