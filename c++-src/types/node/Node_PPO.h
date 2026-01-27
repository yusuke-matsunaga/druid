#ifndef NODE_PPO_H
#define NODE_PPO_H

/// @file Node_PPO.h
/// @brief Node_PPO のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Node_PPO Node_PPO.h "Node_PPO.h"
/// @brief FF入力も含んだ擬似外部出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_PPO :
  public NodeRep
{
protected:

  /// @brief コンストラクタ
  Node_PPO(
    SizeType id,         ///< [in] ノード番号
    SizeType output_id,  ///< [in] 出力番号
    const NodeRep* fanin ///< [in] ファンインのノード
  ) : NodeRep(id, {fanin}),
      mOutputId{output_id}
  {
  }

  /// @brief デストラクタ
  ~Node_PPO() = default;


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
  PrimType
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


//////////////////////////////////////////////////////////////////////
/// @class Node_Output Node_PPO.h "Node_PPO.h"
/// @brief 出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_Output :
  public Node_PPO
{
public:

  /// @brief コンストラクタ
  Node_Output(
    SizeType id,         ///< [in] ノード番号
    SizeType output_id,  ///< [in] 出力番号
    const NodeRep* fanin ///< [in] ファンインのノード
  ) : Node_PPO(id, output_id, fanin)
  {
  }

  /// @brief デストラクタ
  ~Node_Output() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部出力タイプの時 true を返す．
  bool
  is_primary_output() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_DffInput Node_PPO.h "Node_PPO.h"
/// @brief DFFの入力を表すクラス
//////////////////////////////////////////////////////////////////////
class Node_DffInput :
  public Node_PPO
{
public:

  /// @brief コンストラクタ
  Node_DffInput(
    SizeType id,         ///< [in] ノード番号
    SizeType output_id,  ///< [in] 出力番号
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
    const NodeRep* fanin ///< [in] ファンインのノード
  ) : Node_PPO(id, output_id, fanin),
      mDffId{dff_id}
  {
  }

  /// @brief デストラクタ
  ~Node_DffInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF の入力に接続している外部出力タイプの時 true を返す．
  bool
  is_dff_input() const override;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  SizeType
  dff_id() const override;

  /// @brief DFFに関する相方のノードを返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  const NodeRep*
  alt_node() const override;

  /// @brief alt_node を設定する．
  void
  set_alt_node(
    const NodeRep* node
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF 番号
  SizeType mDffId;

  // 相方のノード
  const NodeRep* mAltNode{0};

};

END_NAMESPACE_DRUID

#endif // NODE_PPO_H
