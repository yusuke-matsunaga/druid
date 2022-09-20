#ifndef TPGPPO_H
#define TPGPPO_H

/// @file TpgPPO.h
/// @brief TpgPPO のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgPPO TpgPPO.h "TpgPPO.h"
/// @brief FF入力も含んだ擬似外部出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgPPO :
  public TpgNode
{
protected:

  /// @brief コンストラクタ
  TpgPPO(
    SizeType output_id,  ///< [in] 出力番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgPPO() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_output() || is_dff_input()
  bool
  is_ppo() const override;

  /// @brief 外部出力タイプの時に出力番号を返す．
  ///
  /// node = TpgNetwork::output(node->output_id())
  /// の関係を満たす．
  /// is_output() が false の場合の返り値は不定
  SizeType
  output_id() const override;

  /// @brief TFIサイズの昇順に並べた時の出力番号を返す．
  SizeType
  output_id2() const override;

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ただし疑似出力の場合はバッファタイプとみなす．
  GateType
  gate_type() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号2をセットする．
  ///
  /// 出力ノード以外では無効
  void
  set_output_id2(
    SizeType id ///< [in] セットする番号
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力番号
  SizeType mOutputId;

  // 出力番号2
  SizeType mOutputId2;

};

END_NAMESPACE_DRUID

#endif // TPGPPO_H
