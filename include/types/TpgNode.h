#ifndef TPGNODE_H
#define TPGNODE_H

/// @file TpgNode.h
/// @brief TpgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgObjBase.h"
#include "ym/PrimType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgNode TpgNode.h "TpgNode.h"
/// @brief DRUID 用のノードを表すクラス
/// @sa TpgNetwork
/// @sa TpgMFFC
/// @sa TpgFFR
///
/// 基本的には一つのゲート(セル)に対応しているが，
/// もとのゲートが組み込み型でない場合には複数の TpgNode を組み合わ
/// せてもとのゲートを表す．
///
/// そうする理由は side input の値に対する controlling value，
/// non-controlling value が定まっていたほうがバックトレースが
/// やりやすいから．SAT に関しては complex タイプでも問題なく
/// 処理できる．
///
/// そのため，場合によってはファンインの故障を表すための仮想的な
/// ノードを挿入する場合もある．
//////////////////////////////////////////////////////////////////////
class TpgNode :
  public TpgObjBase
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgNode() = default;

  /// @brief 値を指定したコンストラクタ
  explicit
  TpgNode(
    const std::shared_ptr<NetworkRep>& network, ///< [in] 親のネットワーク
    SizeType id                                 ///< [in] ノード番号
  ) : TpgObjBase(network, id)
  {
  }

  /// @brief デストラクタ
  ~TpgNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const;

  /// @brief ファンインを得る．
  TpgNode
  fanin(
    SizeType index ///< [in] 位置番号 ( 0 <= index < fanin_num() )
  ) const;

  /// @brief ファンインのリストを得る．
  TpgNodeList
  fanin_list() const;

  /// @brief ファンアウト数を得る．
  SizeType
  fanout_num() const;

  /// @brief ファンアウトを得る．
  TpgNode
  fanout(
    SizeType index ///< [in] 位置番号 ( 0 <= index < fanout_num() )
  ) const;

  /// @brief ファンアウトのリストを得る．
  TpgNodeList
  fanout_list() const;

  /// @brief FFR の根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  TpgNode
  ffr_root() const;

  /// @brief MFFCの根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  TpgNode
  mffc_root() const;

  /// @brief 直近の dominator を得る．
  ///
  /// これが不正値の場合は MFFC の根のノードだということ．
  TpgNode
  imm_dom() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 入出力ノードに関係する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力タイプの時 true を返す．
  bool
  is_primary_input() const;

  /// @brief DFF の出力に接続している入力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_input() でもある．
  bool
  is_dff_output() const;

  /// @brief 入力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_input() || is_dff_output()
  bool
  is_ppi() const;

  /// @brief 外部出力タイプの時 true を返す．
  bool
  is_primary_output() const;

  /// @brief DFF の入力に接続している出力タイプの時 true を返す．
  ///
  /// 紛らわしいが is_pseudo_output() でもある．
  bool
  is_dff_input() const;

  /// @brief 出力タイプの時 true を返す．
  ///
  /// 具体的には is_primary_output() || is_dff_input()
  bool
  is_ppo() const;

  /// @brief 外部入力タイプの時に入力番号を返す．
  ///
  /// node = TpgNetwork::ppi(node->input_id())
  /// の関係を満たす．
  /// is_ppi() が false の場合の返り値は不定
  SizeType
  input_id() const;

  /// @brief 外部出力タイプの時に出力番号を返す．
  ///
  /// node = TpgNetwork::ppo(node->output_id())
  /// の関係を満たす．
  /// is_ppo() が false の場合の返り値は不定
  SizeType
  output_id() const;

  /// @brief TFIサイズの昇順に並べた時の出力番号を返す．
  SizeType
  output_id2() const;

  /// @brief 接続している DFF 番号を返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  SizeType
  dff_id() const;

  /// @brief DFFに関する相方のノードを返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  TpgNode
  alt_node() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 論理ノードに関係する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief logic タイプの時 true を返す．
  bool
  is_logic() const;

  /// @brief ゲートタイプを得る．
  ///
  /// - is_logic() が true の時はゲートタイプを返す．
  /// - is_ppo() が true の時は PrimType::Buff を返す．
  /// - それ以外の時は PrimType::None を返す．
  PrimType
  gate_type() const;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  cval() const;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  nval() const;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const;


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
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief NodeRep を返す．
  const NodeRep*
  _node() const
  {
    return TpgBase::_node(id());
  }

};

/// @brief ストリーム出力
inline
std::ostream&
operator<<(
  std::ostream& s,
  const TpgNode& node
)
{
  s << node.str();
  return s;
}


END_NAMESPACE_DRUID

#endif // TPGNODE_H
