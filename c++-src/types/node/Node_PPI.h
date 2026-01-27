#ifndef NODE_PPI_H
#define NODE_PPI_H

/// @file Node_PPI.h
/// @brief Node_PPI のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Node_PPI Node_PPI.h "Node_PPI.h"
/// @brief FF出力も含めた擬似外部入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_PPI :
  public NodeRep
{
protected:

  /// @brief コンストラクタ
  Node_PPI(
    SizeType id,      ///< [in] ノード番号
    SizeType input_id ///< [in] 入力番号
  ) : NodeRep(id, {}),
      mInputId{input_id}
  {
  }

  /// @brief デストラクタ
  ~Node_PPI() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_input() || is_dff_output()
  bool
  is_ppi() const override;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::input(node->input_id()
  /// の関係を満たす．
  /// is_input() が false の場合の返り値は不定
  SizeType
  input_id() const override;

  /// @brief ゲートタイプを得る．
  PrimType
  gate_type() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力番号
  SizeType mInputId;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_Input Node_PPI.h "Node_PPI.h"
/// @brief 入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_Input :
  public Node_PPI
{
public:

  /// @brief コンストラクタ
  Node_Input(
    SizeType id,      ///< [in] ノード番号
    SizeType input_id ///< [in] 入力番号
  ) : Node_PPI(id, input_id)
  {
  }

  /// @brief デストラクタ
  ~Node_Input() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力タイプの時 true を返す．
  bool
  is_primary_input() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_DffOutput Node_PPI.h "Node_PPI.h"
/// @brief DFF の出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_DffOutput :
  public Node_PPI
{
public:

  /// @brief コンストラクタ
  Node_DffOutput(
    SizeType id,       ///< [in] ノード番号
    SizeType input_id, ///< [in] 入力番号
    SizeType dff_id    ///< [in] 接続しているDFFのID番号
  ) : Node_PPI(id, input_id),
      mDffId{dff_id}
  {
  }

  /// @brief デストラクタ
  ~Node_DffOutput() = default;


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
  const NodeRep* mAltNode{nullptr};

};

END_NAMESPACE_DRUID

#endif // NODE_PPI_H
