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
    const TpgNode* fanin ///< [in] ファンインのノード
  ) : TpgNode{{fanin}}
  {
  }

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
  set_output_id(
    SizeType id ///< [in] セットする番号
  )
  {
    mOutputId = id;
  }

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
/// @class TpgOutput TpgOutput.h "TpgOutput.h"
/// @brief 出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgOutput :
  public TpgPPO
{
public:

  /// @brief コンストラクタ
  TpgOutput(
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部出力タイプの時 true を返す．
  bool
  is_primary_output() const override;

};

//////////////////////////////////////////////////////////////////////
/// @class TpgDffControl TpgPPO.h "TpgPPO.h"
/// @brief DFFの制御端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffControl :
  public TpgPPO
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


//////////////////////////////////////////////////////////////////////
/// @class TpgDffInput TpgPPO.h "TpgPPO.h"
/// @brief DFFの入力を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffInput :
  public TpgDffControl
{
public:

  /// @brief コンストラクタ
  TpgDffInput(
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
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

  /// @brief DFFに関する相方のノードを返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  const TpgNode*
  alt_node() const override;

  /// @brief alt_node を設定する．
  void
  set_alt_node(
    const TpgNode* node
  )
  {
    mAltNode = node;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 相方のノード
  const TpgNode* mAltNode{nullptr};

};


//////////////////////////////////////////////////////////////////////
/// @class TpgDffClear TpgPPO.h "TpgPPO.h"
/// @brief DFFのクリア端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffClear :
  public TpgDffControl
{
public:

  /// @brief コンストラクタ
  TpgDffClear(
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
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


//////////////////////////////////////////////////////////////////////
/// @class TpgDffPreset TpgDffPreset.h "TpgDffPreset.h"
/// @brief DFFのプリセット端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffPreset :
  public TpgDffControl
{
public:

  /// @brief コンストラクタ
  TpgDffPreset(
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffPreset() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
  bool
  is_dff_preset() const override;

};

END_NAMESPACE_DRUID

#endif // TPGPPO_H
