#ifndef TPGNODE_H
#define TPGNODE_H

/// @file TpgNode.h
/// @brief TpgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/logic.h" // for PrimType


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
class TpgNode
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  explicit
  TpgNode(
    const vector<const TpgNode*>& fanin_list ///< [in] ファンインリスト
  );

  /// @brief コピーコンストラクタは禁止
  TpgNode(const TpgNode& src) = delete;

  /// @brief コピー代入演算子も禁止
  TpgNode&
  operator=(const TpgNode& src) = delete;

  /// @brief ムーブコンストラクタは禁止
  TpgNode(TpgNode&& src) = delete;

  /// @brief ムーブ代入演算子も禁止
  TpgNode&
  operator=(TpgNode&& src) = delete;

  /// @brief デストラクタ
  virtual
  ~TpgNode() = default;


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

  /// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_clock() const;

  /// @brief DFF のクリア端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_clear() const;

  /// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
  virtual
  bool
  is_dff_preset() const;

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
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  SizeType
  dff_id() const;

  /// @brief DFFに関する相方のノードを返す．
  ///
  /// is_dff_input() | is_dff_output() の時に意味を持つ．
  virtual
  const TpgNode*
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

  /// @brief side-input の値を得る．
  virtual
  Val3
  side_val() const;

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const
  {
    return mFaninList.size();
  }

  /// @brief ファンインを得る．
  const TpgNode*
  fanin(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanin_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < fanin_num() );
    return mFaninList[pos];
  }

  /// @brief ファンインのリストを得る．
  const vector<const TpgNode*>&
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

  /// @brief ファンアウトを得る．
  const TpgNode*
  fanout(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanout_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < fanout_num() );
    return mFanoutList[pos];
  }

  /// @brief ファンアウトのリストを得る．
  const vector<const TpgNode*>&
  fanout_list() const
  {
    return mFanoutList;
  }

  /// @brief FFR の根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const TpgNode*
  ffr_root() const
  {
    if ( fanout_num() == 0 || fanout_num() > 1 ) {
      return this;
    }
    else {
      return fanout(0)->ffr_root();
    }
  }

  /// @brief MFFCの根のノードを得る．
  ///
  /// 自分が根の場合には自分自身を返す．
  const TpgNode*
  mffc_root() const
  {
    if ( imm_dom() == nullptr ) {
      return this;
    }
    else {
      return imm_dom()->mffc_root();
    }
  }

  /// @brief 直近の dominator を得る．
  ///
  /// これが nullptr の場合は MFFC の根のノードだということ．
  const TpgNode*
  imm_dom() const
  {
    return mImmDom;
  }

  /// @brief データパス系のノードの時 true を返す．
  bool
  is_datapath() const
  {
    return mDataPath;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @grief ID番号を設定する．
  void
  set_id(
    SizeType id
  )
  {
    mId = id;
  }

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
    const vector<const TpgNode*>& fo_list
  )
  {
    mFanoutList = fo_list;
  }

  /// @brief immediate dominator をセットする．
  void
  set_imm_dom(
    const TpgNode* dom  ///< [in] dominator ノード
  );

  /// @brief datapath の印をセットする．
  void
  set_datapath(
    bool val ///< [in] 値
  )
  {
    mDataPath = val;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // デバッグ用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード名を表す文字列(Node#id)を返す．
  string
  str() const
  {
    ostringstream buf;
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
  vector<const TpgNode*> mFaninList;

  // ファンアウトの配列
  vector<const TpgNode*> mFanoutList;

  // immediate dominator
  const TpgNode* mImmDom{nullptr};

  // データパスの印
  bool mDataPath{false};

};

END_NAMESPACE_DRUID

#endif // TPGNODE_H
