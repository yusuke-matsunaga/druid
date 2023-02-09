#ifndef TPGDFFOUTPUT_H
#define TPGDFFOUTPUT_H

/// @file TpgDffOutput.h
/// @brief TpgDffOutput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPI.h"
#include "TpgDFF.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffOutput TpgDffOutput.h "TpgDffOutput.h"
/// @brief DFF の出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffOutput :
  public TpgPPI
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgDffOutput(
    SizeType input_id, ///< [in] 入力番号
    SizeType dff_id    ///< [in] 接続しているDFFのID番号
  );

  /// @brief デストラクタ
  ~TpgDffOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF の出力に接続している外部入力タイプの時 true を返す．
  bool
  is_dff_output() const override;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  SizeType
  dff_id() const override;

  /// @brief DFFに関する相方のノードを返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  const TpgNode*
  alt_node() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF 番号
  SizeType mDffId;

  // 相方のノード
  const TpgNode* mAltNode{nullptr};

};

END_NAMESPACE_DRUID

#endif // TPGDFFOUTPUT_H
