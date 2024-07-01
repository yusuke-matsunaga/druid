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
#include "TpgPPI.h"
#include "TpgPPO.h"
#include "TpgGate.h"
#include "GateType.h"
#include "FaultType.h"


BEGIN_NAMESPACE_DRUID

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

  /// @brief 空のコンストラクタ
  TpgNetworkImpl() = default;

  /// @brief コンストラクタ
  explicit
  TpgNetworkImpl(
    const BnModel& model, ///< [in] コピー元のモデル
    FaultType fault_type  ///< [in] 故障の種類
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
    return mMFFCList.size();
  }

  /// @brief MFFC を返す．
  const TpgMFFC*
  mffc(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < mffc_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < mffc_num() );

    return mMFFCList[pos];
  }

  /// @brief node の属している MFFC を返す．
  const TpgMFFC*
  mffc(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    auto root = node->mffc_root();
    return mMFFCMap.at(root->id());
  }

  /// @brief MFFC のリストを返す．
  const vector<const TpgMFFC*>&
  mffc_list() const
  {
    return mMFFCList;
  }

  /// @brief FFR 数を返す．
  SizeType
  ffr_num() const
  {
    return mFFRList.size();
  }

  /// @brief FFR を返す．
  const TpgFFR*
  ffr(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < ffr_num() );

    return mFFRList[pos];
  }

  /// @brief node の属している FFR を返す．
  const TpgFFR*
  ffr(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    auto root = node->ffr_root();
    return mFFRMap.at(root->id());
  }

  /// @brief FFR のリストを返す．
  const vector<const TpgFFR*>&
  ffr_list() const
  {
    return mFFRList;
  }

  /// @brief DFF数を得る．
  SizeType
  dff_num() const
  {
    return mDffInputList.size();
  }

  /// @brief DFFの入力ノードを得る．
  ///
  /// @code
  /// network.dff_input(pos)->dff_id() == pos
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  dff_input(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < dff_num() );

    return mDffInputList[pos];
  }

  /// @brief DFFの出力ノードを得る．
  ///
  /// @code
  /// network.dff_output(pos)->dff_id() == pos
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  dff_output(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < dff_num() );

    return mDffOutputList[pos];
  }

  /// @brief ゲート数を返す．
  SizeType
  gate_num() const
  {
    return mGateList.size();
  }

  /// @brief ゲート情報を得る．
  const TpgGate*
  gate(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < gate_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < gate_num() );
    return mGateList[pos];
  }

  /// @brief ゲート情報のリストを得る．
  const vector<const TpgGate*>&
  gate_list() const
  {
    return mGateList;
  }

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const
  {
    return mFaultType;
  }

  /// @brief 代表故障のリストを得る．
  const vector<const TpgFault*>&
  rep_fault_list() const
  {
    return mRepFaultList;
  }

  /// @brief 故障番号の最大値を得る．
  SizeType
  max_fault_id() const
  {
    return mFaultArray.size();
  }

  /// @brief ステムの故障を得る.
  ///
  /// 故障タイプが網羅故障の場合には不正な呼び出しとなる．
  const TpgFault*
  find_fault(
    const TpgGate* gate, ///< [in] 出力のゲート
    Fval2 fval           ///< [in] 故障値
  ) const;

  /// @brief ブランチの故障を得る．
  ///
  /// 故障タイプが網羅故障の場合には不正な呼び出しとなる．
  const TpgFault*
  find_fault(
    const TpgGate* gate, ///< [in] 出力のゲート
    SizeType ipos,       ///< [in] 入力位置
    Fval2 fval           ///< [in] 故障値
  ) const;

  /// @brief 網羅故障を得る.
  ///
  /// 故障タイプが網羅故障でない場合には不正な呼び出しとなる．
  const TpgFault*
  find_fault(
    const TpgGate* gate,      ///< [in] 出力のゲート
    const vector<bool>& ivals ///< [in] 入力値のベクトル
  ) const;


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

  /// @brief FFR を作る．
  const TpgFFR*
  new_ffr(
    const TpgNode* root ///< [in] FFR の根のノード
  );

  /// @brief MFFC を作る．
  void
  new_mffc(
    const TpgNode* root, ///< [in] root MFFCの根のノード
    const unordered_map<SizeType, const TpgFFR*>& ffr_map ///< [in] ノード番号をキーにしてFFRを格納する辞書
  );

  /// @brief set() の後処理
  void
  post_op(
    const TpgConnectionList& connection_list ///< [in] 接続リスト
  );

  /// @brief 故障を取り出す．
  const TpgFault*
  _find_fault(
    SizeType key
  ) const
  {
    if ( mFaultDict.count(key) > 0 ) {
      return mFaultDict.at(key);
    }
    return nullptr;
  }

  /// @brief ゲートに関連した故障を作る．
  void
  gen_gate_faults(
    const TpgGate* gate,        ///< [in] 対象のゲート
    vector<SizeType>& fault_map ///< [out] 各ノードの出力の故障を記録する配列
  );

  /// @brief ステムの故障を作る．
  void
  gen_stem_fault(
    const TpgGate* gate,        ///< [in] 対象のゲート
    vector<SizeType>& fault_map ///< [out] 各ノードの出力の故障を記録する配列
  );

  /// @brief ブランチの故障を作る．
  void
  gen_branch_fault(
    const TpgGate* gate ///< [in] 対象のゲート
  );

  /// @brief ゲート網羅故障を作る．
  void
  gen_ex_fault(
    const TpgGate* gate  ///< [in] 対象のゲート
  );

  /// @brief 故障を登録する．
  void
  reg_fault(
    TpgFault* fault ///< [in] 故障
  );

  /// @brief ステムの故障用のキーを作る．
  SizeType
  gen_key(
    const TpgGate* gate, ///< [in] 対象のゲート
    Fval2 fval           ///< [in] 故障値
  ) const;

  /// @brief ブランチの故障用のキーを作る．
  SizeType
  gen_key(
    const TpgGate* gate, ///< [in] 対象のゲート
    SizeType ipos,       ///< [in] 入力位置
    Fval2 fval           ///< [in] 故障値
  ) const;

  /// @brief ゲート網羅故障用のキーを作る．
  SizeType
  gen_key(
    const TpgGate* gate,      ///< [in] 対象のゲート
    const vector<bool>& ivals ///< [in] 入力値のベクタ
  ) const;

  /// @brief 代表故障を求める．
  void
  set_rep_fault(
    const TpgGate* gate,               ///< [in] 対象のゲート
    const vector<SizeType>& fault_map, ///< [in] 各ノードの出力の故障番号を記録した配列
    vector<SizeType>& rep_map          ///< [out] 代表故障番号を記録する配列
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

  // ノードのポインタ配e列
  vector<const TpgNode*> mNodeArray;

  // ゲートに関する情報の配列
  vector<const TpgGate*> mGateList;

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

  // DFFの入力ノードのリスト
  vector<TpgDffInput*> mDffInputList;

  // DFFの出力ノードのリスト
  vector<TpgDffOutput*> mDffOutputList;

  // MFFC のリスト
  vector<const TpgMFFC*> mMFFCList;

  // MFFC の根のノード番号をキーにして MFFC を格納する辞書
  unordered_map<SizeType, const TpgMFFC*> mMFFCMap;

  // FFR のリスト
  vector<const TpgFFR*> mFFRList;

  // FFR の根のノード番号をキーにして FFR を格納する辞書
  unordered_map<SizeType, const TpgFFR*> mFFRMap;

  // 故障の種類
  FaultType mFaultType{FaultType::None};

  // 故障の配列
  vector<TpgFault*> mFaultArray;

  // 代表故障のリスト
  vector<const TpgFault*> mRepFaultList;

  // 故障の辞書
  std::unordered_map<SizeType, const TpgFault*> mFaultDict;

};

END_NAMESPACE_DRUID

#endif // TPGNETWORKIMPL_H
