#ifndef TPGPPI_H
#define TPGPPI_H

/// @file TpgPPI.h
/// @brief TpgPPI のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgPPI TpgPPI.h "TpgPPI.h"
/// @brief FF出力も含めた擬似外部入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgPPI :
  public TpgNode
{
protected:

  /// @brief コンストラクタ
  TpgPPI(
  ) : TpgNode{{}}
  {
  }

  /// @brief デストラクタ
  ~TpgPPI() = default;


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


public:
  //////////////////////////////////////////////////////////////////////
  // 設定用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力番号を設定する．
  void
  set_input_id(
    SizeType id
  )
  {
    mInputId = id;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力番号
  SizeType mInputId;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgInput TpgInput.h "TpgInput.h"
/// @brief 入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgInput :
  public TpgPPI
{
public:

  /// @brief コンストラクタ
  TpgInput() = default;

  /// @brief デストラクタ
  ~TpgInput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力タイプの時 true を返す．
  bool
  is_primary_input() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgDffOutput TpgDffOutput.h "TpgDffOutput.h"
/// @brief DFF の出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffOutput :
  public TpgPPI
{
public:

  /// @brief コンストラクタ
  TpgDffOutput(
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

  // 対応する DFF 番号
  SizeType mDffId;

  // 相方のノード
  const TpgNode* mAltNode{nullptr};

};

END_NAMESPACE_DRUID

#endif // TPGPPI_H
