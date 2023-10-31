#ifndef TPGNETWORKIMPL_H
#define TPGNETWORKIMPL_H

/// @file TpgNetworkImpl.h
/// @brief TpgNetworkImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "ym/bn.h"
#include "ym/clib.h"
#include "ym/logic.h"
#include "ym/Array.h"
#include "TpgGate.h"
#include "DFFImpl.h"
#include "MFFCImpl.h"
#include "FFRImpl.h"
#include "GateType.h"


BEGIN_NAMESPACE_DRUID

class TpgPPI;
class TpgPPO;
class TpgGateImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgNetworkImpl TpgNetworkImpl.h "TpgNetworkImpl.h"
/// @brief TpgNetwork の実装クラス
//////////////////////////////////////////////////////////////////////
class TpgNetworkImpl
{
private:

  // ノードの接続を表すクラス
  using TpgConnectionList = vector<vector<const TpgNode*>>;


public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  TpgNetworkImpl();

  /// @brief コンストラクタ
  explicit
  TpgNetworkImpl(
    const BnModel& model ///< [in] コピー元のモデル
  );

  /// @brief デストラクタ
  ~TpgNetworkImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 通常の構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を得る．
  SizeType
  node_num() const
  {
    return mNodeArray.size();
  }

  /// @brief ノードを得る．
  ///
  /// @code
  /// node = network.node(node->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  node(
    SizeType id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const
  {
    ASSERT_COND( id >= 0 && id < node_num() );

    return mNodeArray[id];
  }

  /// @brief 全ノードのリストを得る．
  const vector<const TpgNode*>&
  node_list() const
  {
    return mNodeArray;
  }

  /// @brief ノード名を得る．
  const string&
  node_name(
    SizeType id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const;

  /// @brief 外部入力数を得る．
  SizeType
  input_num() const
  {
    return mInputNum;
  }

  /// @brief 外部入力ノードを得る．
  ///
  /// @code
  /// node = network.input(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  input(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < input_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < input_num() );

    return mPPIArray[pos];
  }

  /// @brief 外部入力ノードのリストを得る．
  const vector<const TpgNode*>&
  input_list() const
  {
    return mPPIArray;
  }

  /// @brief 外部出力数を得る．
  SizeType
  output_num() const
  {
    return mOutputNum;
  }

  /// @brief 外部出力ノードを得る．
  ///
  /// @code
  /// node = network.output(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < output_num() );

    return mPPOArray[pos];
  }

  /// @brief 外部出力ノードのリストを得る．
  const vector<const TpgNode*>&
  output_list() const
  {
    return mPPOArray;
  }

  /// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
  ///
  /// @code
  /// node = network.output2(node->output_id2())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output2(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < output_num() );

    return mPPOArray2[pos];
  }

  /// @brief スキャン方式の擬似外部入力数を得る．
  ///
  /// = input_num() + dff_num()
  SizeType
  ppi_num() const
  {
    return input_num() + dff_num();
  }

  /// @brief スキャン方式の擬似外部入力を得る．
  ///
  /// @code
  /// node = network.ppi(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppi(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ppi_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < ppi_num() );

    return mPPIArray[pos];
  }

  /// @brief PPI の名前を返す．
  string
  ppi_name(
    SizeType input_id ///< [in] 入力番号
  ) const
  {
    ASSERT_COND( 0 <= input_id && input_id < ppi_num() );
    return mPPINameArray[input_id];
  }

  /// @brief 擬似外部入力のリストを得る．
  const vector<const TpgNode*>&
  ppi_list() const
  {
    return mPPIArray;
  }

  /// @brief スキャン方式の擬似外部出力数を得る．
  ///
  /// = output_num() + dff_num()
  SizeType
  ppo_num() const
  {
    return output_num() + dff_num();
  }

  /// @brief スキャン方式の擬似外部出力を得る．
  ///
  /// @code
  /// node = network.ppo(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppo(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ppo_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < ppo_num() );

    return mPPOArray[pos];
  }

  /// @brief PPO の名前を返す．
  string
  ppo_name(
    SizeType output_id ///< [in] 出力番号
  ) const
  {
    ASSERT_COND( 0 <= output_id && output_id < ppo_num() );

    return mPPONameArray[output_id];
  }

  /// @brief 擬似外部出力のリストを得る．
  const vector<const TpgNode*>&
  ppo_list() const
  {
    return mPPOArray;
  }

  /// @brief MFFC 数を返す．
  SizeType
  mffc_num() const
  {
    return mMFFCArray.size();
  }

  /// @brief MFFC を返す．
  const MFFCImpl&
  _mffc(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < mffc_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < mffc_num() );

    return mMFFCArray[pos];
  }

  /// @brief FFR 数を返す．
  SizeType
  ffr_num() const
  {
    return mFFRArray.size();
  }

  /// @brief FFR を返す．
  const FFRImpl&
  _ffr(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < ffr_num() );

    return mFFRArray[pos];
  }

  /// @brief DFF数を得る．
  SizeType
  dff_num() const
  {
    return mDFFArray.size();
  }

  /// @brief DFF を得る．
  ///
  /// @code
  /// dff = network.dff(dff->id())
  /// @endcode
  /// の関係が成り立つ．
  const DFFImpl&
  _dff(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < dff_num() );

    return mDFFArray[pos];
  }

  /// @brief DFF のリストを得る．
  const vector<DFFImpl>&
  dff_list() const { return mDFFArray; }

  /// @brief ゲート数を返す．
  SizeType
  gate_num() const
  {
    return mGateArray.size();
  }

  /// @brief ゲート情報を得る．
  const TpgGateImpl*
  _gate(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < gate_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < gate_num() );
    return mGateArray[pos];
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定するための関数
  //////////////////////////////////////////////////////////////////////

  /// @brief サイズを設定する．
  SizeType
  set_size(
    SizeType input_num,
    SizeType output_num,
    SizeType dff_num,
    SizeType gate_num,
    SizeType extra_node_num
  );

  /// @brief set() の後処理
  void
  post_op(
    const TpgConnectionList& connection_list ///< [in] 接続リスト
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief 入力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgPPI*
  make_input_node(
    const string& name ///< [in] ノード名
  );

  /// @brief 出力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgPPO*
  make_output_node(
    const string& name,  ///< [in] ノード名
    const TpgNode* inode ///< [in] 入力のノード
  );

  /// @brief DFFの入力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgPPO*
  make_dff_input_node(
    SizeType dff_id,     ///< [in] DFF番号
    const string& name,  ///< [in] ノード名
    const TpgNode* inode ///< [in] 入力のノード
  );

  /// @brief DFFの出力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgPPI*
  make_dff_output_node(
    SizeType dff_id,   ///< [in] DFF番号
    const string& name ///< [in] ノード名
  );

  /// @brief 論理ノードを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_logic_node(
    const GateType* gate_type,                ///< [in] ゲートの種類
    const vector<const TpgNode*>& fanin_list, ///< [in] ファンインのリスト
    TpgConnectionList& connection_list        ///< [in] 接続リスト
  );

  /// @brief 論理式から TpgNode の木を生成する．
  ///
  /// leaf_nodes は 変数番号 * 2 + (0/1) に
  /// 該当する変数の肯定/否定のリテラルが入っている．
  TpgNode*
  make_cplx_node(
    const Expr& expr,			      ///< [in] 式
    const vector<const TpgNode*>& leaf_nodes, ///< [in] 式のリテラルに対応するノードの配列
    vector<TpgGate::BranchInfo>& branch_info, ///< [in] ブランチの情報
    TpgConnectionList& connection_list        ///< [out] 接続リスト
  );

  /// @brief バッファを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_buff_node(
    const TpgNode* fanin,	       ///< [in] ファンインのノード
    TpgConnectionList& connection_list ///< [out] 接続リスト
  );

  /// @brief インバーターを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_not_node(
    const TpgNode* fanin,		            ///< [in] ファンインのノード
    TpgConnectionList& connection_list ///< [out] 接続リスト
  );

  /// @brief 組み込み型の論理ゲートを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_prim_node(
    PrimType type,			      ///< [in] ゲートの型
    const vector<const TpgNode*>& fanin_list, ///< [in] ファンインのリスト
    TpgConnectionList& connection_list        ///< [out] 接続リスト
  );

  /// @brief 論理ノードを作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_logic(
    PrimType gate_type,                      ///< [in] ゲートタイプ
    const vector<const TpgNode*>& inode_list ///< [in] 入力ノードのリスト
  );

  /// @brief PPI系のノードの登録
  void
  reg_ppi(
    TpgPPI* node,      ///< [in] ノード
    const string& name ///< [in] 名前
  );

  /// @brief PPO系のノードの登録
  void
  reg_ppo(
    TpgPPO* node,      ///< [in] ノード
    const string& name ///< [in] 名前
  );

  /// @brief 全てのノードの登録
  void
  reg_node(
    TpgNode* node ///< [in] ノード
  );

  /// @brief FFR の情報を設定する．
  void
  set_ffr(
    SizeType id,        ///< [in] ID番号
    const TpgNode* root ///< [in] FFR の根のノード
  );

  /// @brief MFFC の情報を設定する．
  void
  set_mffc(
    SizeType id,        ///< [in] ID番号
    const TpgNode* root, ///< [in] root MFFCの根のノード
    const unordered_map<SizeType, SizeType>& ffr_map ///< [in] ノード番号をキーにしてFFR番号を格納する辞書
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  SizeType mInputNum{0};

  // 出力数
  SizeType mOutputNum{0};

  // ゲートの情報を管理するオブジェクト
  GateTypeMgr mGateTypeMgr;

  // DFFの実体の配列
  vector<DFFImpl> mDFFArray;

  // ノードのポインタ配列
  vector<const TpgNode*> mNodeArray;

  // ゲートに関する情報の配列
  vector<TpgGateImpl*> mGateArray;

  // PPIノードの配列
  vector<const TpgNode*> mPPIArray;

  // PPIの名前の配列
  vector<string> mPPINameArray;

  // PPOノードの配列
  vector<const TpgNode*> mPPOArray;

  // PPOの名前の配列
  vector<string> mPPONameArray;

  // TFI サイズの降順に整列したPPOノードの配列
  vector<const TpgNode*> mPPOArray2;

  // MFFC の本体の配列
  vector<MFFCImpl> mMFFCArray;

  // FFR の本体の配列
  vector<FFRImpl> mFFRArray;

};

END_NAMESPACE_DRUID

#endif // TPGNETWORKIMPL_H
