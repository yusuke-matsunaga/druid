#ifndef TPGDFFCONTROL_H
#define TPGDFFCONTROL_H

/// @file TpgDffControl.h
/// @brief TpgDffControl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"
#include "TpgDFF.h"


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
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffControl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  SizeType
  dff_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF 番号
  SizeType mDffId;

};

END_NAMESPACE_DRUID

#endif // TPGDFFCONTROL_H
