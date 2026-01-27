#ifndef NODEREP_H
#define NODEREP_H

/// @file NodeRep.h
/// @brief NodeRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/PrimType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeRep NodeRep.h "NodeRep.h"
/// @brief TpgNode の実体を表すクラス
//////////////////////////////////////////////////////////////////////
class NodeRep
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  explicit
  NodeRep(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief 入力ノードを作る．
  static
  NodeRep*
  new_input(
    SizeType id,      ///< [in] ノード番号
    SizeType input_id ///< [in] 入力番号
  );

  /// @brief DFFの出力ノードを作る．
  static
  NodeRep*
  new_dff_output(
    SizeType id,       ///< [in] ノード番号
    SizeType input_id, ///< [in] 入力番号
    SizeType dff_id    ///< [in] DFF番号
  );

  /// @grief 出力ノードを作る．
  static
  NodeRep*
  new_output(
    SizeType id,         ///< [in] ノード番号
    SizeType output_id,  ///< [in] 出力番号
    const NodeRep* fanin ///< [in] ファンインのノード
  );

  /// @brief DFFの入力ノードを作る．
  static
  NodeRep*
  new_dff_input(
    SizeType id,         ///< [in] ノード番号
    SizeType output_id,  ///< [in] 出力番号
    SizeType dff_id,     ///< [in] DFF番号
    const NodeRep* fanin ///< [in] ファンインのノード
  );

  /// @brief 論理ノードを作る．
  static
  NodeRep*
  new_logic(
    SizeType id,                                  ///< [in] ノード番号
    PrimType gate_type,                           ///< [in] ゲートの型
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief デストラクタ
  virtual
  ~NodeRep() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  SizeType
  id() const
  {
    return mId;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 入出力ノードに関係する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力タイプの時 true を返す．
  virtual
  bool
  is_primary_input() const;

  /// @brief 外部出力タイプの時 true を返す．
  virtual
  bool
  is_primary_output() const;

  /// @brief DFF の入力に接続している出力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_output() でもある．
  virtual
  bool
  is_dff_input() const;

  /// @brief DFF の出力に接続している入力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_input() でもある．
  virtual
  bool
  is_dff_output() const;

  /// @brief 入力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_input() || is_dff_output()
  virtual
  bool
  is_ppi() const;

  /// @brief 出力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_output() || is_dff_input()
  virtual
  bool
  is_ppo() const;

  /// @brief logic タイプの時 true を返す．
  virtual
  bool
  is_logic() const;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::ppi(node->input_id())
  /// の関係を満たす．
  /// is_ppi() が false の場合の返り値は不定
  virtual
  SizeType
  input_id() const;

  /// @brief 外部出力タイプの時に出力番号を返す．
  ///
  /// node = TpgNetwork::ppo(node->output_id())
  /// の関係を満たす．
  /// is_ppo() が false の場合の返り値は不定
  virtual
  SizeType
  output_id() const;

  /// @brief TFIサイズの昇順に並べた時の出力番号を返す．
  virtual
  SizeType
  output_id2() const;

  /// @brief 接続している DFF 番号を返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  virtual
  SizeType
  dff_id() const;

  /// @brief DFFに関する相方のノードの喉番号を返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  virtual
  const NodeRep*
  alt_node() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードに関係する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// - is_logic() が true の時はゲートタイプを返す．
  /// - is_ppo() が true の時は PrimType::Buff を返す．
  /// - それ以外の時は PrimType::None を返す．
  virtual
  PrimType
  gate_type() const;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  cval() const;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  nval() const;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  coval() const;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  virtual
  Val3
  noval() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報に関するアクセス関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const
  {
    return mFaninList.size();
  }

  /// @brief ファンインのノードを得る．
  const NodeRep*
  fanin(
    SizeType index ///< [in] 位置番号 ( 0 <= index < fanin_num() )
  ) const
  {
    if ( index >= fanin_num() ) {
      throw std::out_of_range{"index is out of range"};
    }
    return mFaninList[index];
  }

  /// @brief ファンインのノードのリストを得る．
  const std::vector<const NodeRep*>&
  fanin_list() const
  {
    return mFaninList;
  }

  /// @brief ファンアウト数を得る．
  SizeType
  fanout_num() const
  {
    return mFanoutList.size();
  }

  /// @brief ファンアウトのノードを得る．
  const NodeRep*
  fanout(
    SizeType index ///< [in] 位置番号 ( 0 <= index < fanout_num() )
  ) const
  {
    if ( index >= fanout_num() ) {
      throw std::out_of_range{"index is out of range"};
    }
    return mFanoutList[index];
  }

  /// @brief ファンアウトのノードのリストを得る．
  const std::vector<const NodeRep*>&
  fanout_list() const
  {
    return mFanoutList;
  }

  /// @brief FFR の根のノードのノード番号を得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const NodeRep*
  ffr_root() const
  {
    if ( fanout_num() == 0 || fanout_num() > 1 ) {
      return this;
    }
    return fanout(0)->ffr_root();
  }

  /// @brief MFFCの根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const NodeRep*
  mffc_root() const
  {
    if ( imm_dom() == nullptr ) {
      return this;
    }
    return imm_dom()->mffc_root();
  }

  /// @brief 直近の dominator のノード番号を得る．
  const NodeRep*
  imm_dom() const
  {
    return mImmDom;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力番号2をセットする．
  ///
  /// 出力ノード以外では無効
  virtual
  void
  set_output_id2(
    SizeType id  ///< [in] セットする番号
  );

  /// @brief ファンアウトを設定する．
  void
  set_fanouts(
    const std::vector<const NodeRep*>& fo_list
  )
  {
    mFanoutList = fo_list;
  }

  /// @brief immediate dominator をセットする．
  void
  set_imm_dom(
    const NodeRep* dom  ///< [in] dominator のノード
  );

  /// @brief DFF に関連したノードの相方のノードをセットする．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  virtual
  void
  set_alt_node(
    const NodeRep* node ///< [in] 相方のノード
  );


public:
  //////////////////////////////////////////////////////////////////////
  // デバッグ用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード名を表す文字列(Node#id)を返す．
  std::string
  str() const
  {
    std::ostringstream buf;
    buf << "Node#" << id();
    return buf.str();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId{0};

  // ファンインの配列
  std::vector<const NodeRep*> mFaninList;

  // ファンアウトのノード番号の配列
  std::vector<const NodeRep*> mFanoutList;

  // immediate dominator
  const NodeRep* mImmDom{nullptr};

};

END_NAMESPACE_DRUID

#endif // NODEREP_H
