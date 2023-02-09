#ifndef TPGDFFINPUT_H
#define TPGDFFINPUT_H

/// @file TpgDffInput.h
/// @brief TpgDffInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPO.h"
#include "TpgDFF.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffInput TpgDffInput.h "TpgDffInput.h"
/// @brief DFFの入力を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffInput :
  public TpgPPO
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgDffInput(
    SizeType output_id,  ///< [in] 出力番号
    TpgDFF dff,          ///< [in] 接続しているDFF
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF の入力に接続している外部出力タイプの時 true を返す．
  bool
  is_dff_input() const override;

#if 0
  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  TpgDFF
  dff() const override;
#endif

  /// @brief DFFに関する相方のノードを返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  const TpgNode*
  alt_node() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF
  TpgDFF mDff;

  // 相方のノード
  const TpgNode* mAltNode{nullptr};

};

END_NAMESPACE_DRUID

#endif // TPGDFFINPUT_H
