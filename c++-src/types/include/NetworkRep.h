#ifndef NETWORK_REP_H
#define NETWORK_REP_H

/// @file NetworkRep.h
/// @brief NetworkRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "ym/bn.h"
#include "ym/clib.h"
#include "ym/logic.h"
#include "ym/Array.h"
#include "types/TpgGate.h"
#include "types/FaultType.h"
#include "NodeRep.h"
#include "GateType.h"
#include "GateRep.h"
#include "FFRRep.h"
#include "MFFCRep.h"
#include "FaultRep.h"


BEGIN_NAMESPACE_DRUID

class FFRRep;
class MFFCRep;
class FaultRep;

//////////////////////////////////////////////////////////////////////
/// @class NetworkRep NetworkRep.h "NetworkRep.h"
/// @brief TpgNetwork の実装クラス
//////////////////////////////////////////////////////////////////////
class NetworkRep
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  ///
  /// サイズはヒントとして用いられる．
  explicit
  NetworkRep(
    FaultType fault_type,       ///< [in] 故障の種類
    SizeType input_num = 0,     ///< [in] 入力数
    SizeType output_num = 0,    ///< [in] 出力数
    SizeType dff_num = 0,       ///< [in] DFF数
    SizeType gate_num = 0,      ///< [in] ゲート数
    SizeType extra_node_num = 0 ///< [in] 追加のノード数
  );

  /// @brief デストラクタ
  ~NetworkRep() = default;


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
  const NodeRep*
  node(
    SizeType id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const
  {
    _check_node_id(id);
    return mNodeArray[id].get();
  }

  /// @brief 全ノードのリストを得る．
  const std::vector<std::unique_ptr<NodeRep>>&
  node_list() const
  {
    return mNodeArray;
  }

  /// @brief ノード名を得る．
  const std::string&
  node_name(
    SizeType id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const;

  /// @brief 外部入力数を得る．
  SizeType
  input_num() const
  {
    return ppi_num() - dff_num();
  }

  /// @brief PPI数を得る．
  SizeType
  ppi_num() const
  {
    return mPPIArray.size();
  }

  /// @brief 外部入力ノードを得る．
  ///
  /// @code
  /// node = network.input(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const NodeRep*
  input(
    SizeType id ///< [in] 入力番号 ( 0 <= id < input_num() )
  ) const
  {
    _check_input_id(id);
    return mPPIArray[id];
  }

  /// @brief 外部入力ノードのリストを得る．
  const std::vector<const NodeRep*>
  input_list() const
  {
    return mPPIArray;
  }

  /// @brief 外部出力数を得る．
  SizeType
  output_num() const
  {
    return ppo_num() - dff_num();
  }

  /// @brief PPO数を得る．
  SizeType
  ppo_num() const
  {
    return mPPOArray.size();
  }

  /// @brief 外部出力ノードを得る．
  ///
  /// @code
  /// node = network.output(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const NodeRep*
  output(
    SizeType id ///< [in] 出力番号 ( 0 <= id < output_num() )
  ) const
  {
    _check_output_id(id);
    return mPPOArray[id];
  }

  /// @brief 外部出力ノードのリストを得る．
  const std::vector<const NodeRep*>
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
  const NodeRep*
  output2(
    SizeType id ///< [in] 出力番号 ( 0 <= id < output_num() )
  ) const
  {
    _check_output_id(id);
    return mPPOArray2[id];
  }

  /// @brief スキャン方式の擬似外部入力を得る．
  ///
  /// @code
  /// node = network.ppi(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const NodeRep*
  ppi(
    SizeType id ///< [in] PPI番号 ( 0 <= id < ppi_num() )
  ) const
  {
    _check_ppi_id(id);
    return mPPIArray[id];
  }

  /// @brief PPI の名前を返す．
  std::string
  ppi_name(
    SizeType id ///< [in] PPI番号 ( 0 <= id < ppi_num() )
  ) const
  {
    _check_ppi_id(id);
    return mPPINameArray[id];
  }

  /// @brief 擬似外部入力のリストを得る．
  const std::vector<const NodeRep*>&
  ppi_list() const
  {
    return mPPIArray;
  }

  /// @brief スキャン方式の擬似外部出力を得る．
  ///
  /// @code
  /// node = network.ppo(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const NodeRep*
  ppo(
    SizeType id ///< [in] PPO番号 ( 0 <= id < ppo_num() )
  ) const
  {
    _check_ppo_id(id);
    return mPPOArray[id];
  }

  /// @brief PPO の名前を返す．
  std::string
  ppo_name(
    SizeType id ///< [in] PPO番号 ( 0 <= id < ppo_num() )
  ) const
  {
    _check_ppo_id(id);
    return mPPONameArray[id];
  }

  /// @brief 擬似外部出力のリストを得る．
  const std::vector<const NodeRep*>&
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
  const MFFCRep*
  mffc(
    SizeType id ///< [in] MFFC番号 ( 0 <= id < mffc_num() )
  ) const
  {
    _check_mffc_id(id);
    return mMFFCArray[id].get();
  }

  /// @brief node の属している MFFC を返す．
  const MFFCRep*
  mffc(
    const NodeRep* node ///< [in] 対象のノード
  ) const
  {
    auto root = node->mffc_root();
    return mMFFCMap.at(root->id());
  }

  /// @brief MFFC のリストを返す．
  const std::vector<std::unique_ptr<MFFCRep>>&
  mffc_list() const
  {
    return mMFFCArray;
  }

  /// @brief FFR 数を返す．
  SizeType
  ffr_num() const
  {
    return mFFRArray.size();
  }

  /// @brief FFR を返す．
  const FFRRep*
  ffr(
    SizeType id ///< [in] FFR番号 ( 0 <= id < ffr_num() )
  ) const
  {
    _check_ffr_id(id);
    return mFFRArray[id].get();
  }

  /// @brief node の属している FFR を返す．
  const FFRRep*
  ffr(
    const NodeRep* node ///< [in] 対象のノード
  ) const
  {
    auto root = node->ffr_root();
    return mFFRMap.at(root->id());
  }

  /// @brief FFR のリストを返す．
  const std::vector<std::unique_ptr<FFRRep>>&
  ffr_list() const
  {
    return mFFRArray;
  }

  /// @brief DFF数を得る．
  SizeType
  dff_num() const
  {
    return mDffList.size();
  }

  /// @brief DFFの入力ノードを得る．
  ///
  /// @code
  /// network.dff_input(pos)->dff_id() == pos
  /// @endcode
  /// の関係が成り立つ．
  const NodeRep*
  dff_input(
    SizeType id ///< [in] DFF番号 ( 0 <= id < dff_num() )
  ) const
  {
    _check_dff_id(id);
    return mDffList[id].first;
  }

  /// @brief DFFの出力ノードを得る．
  ///
  /// @code
  /// network.dff_output(pos)->dff_id() == pos
  /// @endcode
  /// の関係が成り立つ．
  const NodeRep*
  dff_output(
    SizeType id ///< [in] DFF番号 ( 0 <= id < dff_num() )
  ) const
  {
    _check_dff_id(id);
    return mDffList[id].second;
  }

  /// @brief GateType を取り出す．
  const GateType*
  gate_type(
    SizeType id ///< [in] ID番号
  ) const
  {
    _check_gate_type_id(id);
    return mGateTypeArray[id].get();
  }

  /// @brief ゲート数を返す．
  SizeType
  gate_num() const
  {
    return mGateArray.size();
  }

  /// @brief ゲート情報を得る．
  const GateRep*
  gate(
    SizeType id ///< [in] ゲート番号 ( 0 <= id < gate_num() )
  ) const
  {
    _check_gate_id(id);
    return mGateArray[id].get();
  }

  /// @brief ゲート情報のリストを得る．
  const std::vector<std::unique_ptr<GateRep>>&
  gate_list() const
  {
    return mGateArray;
  }

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const
  {
    return mFaultType;
  }

  /// @brief 故障を得る．
  const FaultRep*
  fault(
    SizeType fault_id ///< [in] 故障番号
  ) const
  {
    _check_fault_id(fault_id);
    return mFaultArray[fault_id].get();
  }

  /// @brief 代表故障の故障番号のリストを得る．
  const std::vector<SizeType>&
  rep_fid_list() const
  {
    return mRepFidList;
  }

  /// @brief 故障番号の最大値を得る．
  SizeType
  max_fault_id() const
  {
    return mFaultArray.size();
  }

  /// @brief TFO のノードを求める．
  std::vector<const NodeRep*>
  get_tfo_list(
    const NodeRep* root,           ///< [in] 起点となるノード
    const NodeRep* block = nullptr ///< [in] ブロックノード
  ) const
  {
    return get_tfo_list({root}, block, [](const NodeRep*){});
  }

  /// @brief TFO のノードを求める．
  std::vector<const NodeRep*>
  get_tfo_list(
    const NodeRep* root,                   ///< [in] 起点となるノード
    std::function<void(const NodeRep*)> op ///< [in] ノードに対するファンクタ
  ) const
  {
    return get_tfo_list(std::vector<const NodeRep*>{root}, op);
  }

  /// @brief TFO のノードを求める．
  std::vector<const NodeRep*>
  get_tfo_list(
    const std::vector<const NodeRep*>& root_list, ///< [in] 起点となるノードのリスト
    std::function<void(const NodeRep*)> op        ///< [in] ノードに対するファンクタ
  ) const
  {
    return get_tfo_list(root_list, nullptr, op);
  }

  /// @brief TFO のノードを求める．
  std::vector<const NodeRep*>
  get_tfo_list(
    const std::vector<const NodeRep*>& root_list, ///< [in] 起点となるノードのリスト
    const NodeRep* block,                         ///< [in] ブロックノード
    std::function<void(const NodeRep*)> op        ///< [in] ノードに対するファンクタ
  ) const;

  /// @brief TFI のノードを求める．
  std::vector<const NodeRep*>
  get_tfi_list(
    const std::vector<const NodeRep*>& root_list ///< [in] 起点となるノード
  ) const
  {
    return get_tfi_list(root_list, [](const NodeRep*){});
  }

  /// @brief TFI のノードを求める．
  std::vector<const NodeRep*>
  get_tfi_list(
    const std::vector<const NodeRep*>& root_list, ///< [in] 起点となるノード
    std::function<void(const NodeRep*)> op        ///< [in] ノードに対するファンクタ
  ) const;

  /// @brief 出力からの DFS を行う．
  void
  dfs(
    const std::vector<const NodeRep*>& root_list, ///< [in] 起点となるノード
    std::function<void(const NodeRep*)> pre_func, ///< [in] pre-order の処理関数
    std::function<void(const NodeRep*)> post_func ///< [in] post-order の処理関数
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定するための関数
  //////////////////////////////////////////////////////////////////////

  /// @brief サイズを設定する．
  void
  set_size(
    SizeType input_num,     ///< [in] 入力数
    SizeType output_num,    ///< [in] 出力数
    SizeType dff_num,       ///< [in] DFF数
    SizeType gate_num,      ///< [in] ゲート数
    SizeType extra_node_num ///< [in] 追加のノード数
  );

  /// @brief 入力ノードを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_input_node(
    const std::string& name ///< [in] ノード名
  );

  /// @brief DFFの出力ノードを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_dff_output_node(
    const std::string& name ///< [in] 入力ノード名
  );

  /// @brief 出力ノードを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_output_node(
    const std::string& name, ///< [in] ノード名
    const NodeRep* inode     ///< [in] 入力のノード
  );

  /// @brief DFFの入力ノードを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_dff_input_node(
    SizeType dff_id,         ///< [in] DFF番号
    const std::string& name, ///< [in] ノード名
    const NodeRep* inode     ///< [in] 入力のノード
  );

  /// @brief 組み込み型の論理ゲートを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_prim_node(
    PrimType type,		                  ///< [in] ゲートの型
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief 組込み型の GateType を生成し，登録する．
  const GateType*
  make_gate_type(
    SizeType input_num, ///< [in] 入力数
    PrimType prim_type  ///< [in] 組込み型
  );

  /// @brief 複合型の GateType を生成し，登録する．
  const GateType*
  make_gate_type(
    SizeType input_num, ///< [in] 入力数
    const Expr& expr    ///< [in] 論理式
  );

  /// @brief ゲートを生成する．
  GateRep*
  make_gate(
    const GateType* gate_type, ///< [in] ゲートの種類
    const NodeRep* node,       ///< [in] 出力に対応するノード
    const std::vector<GateRep::BranchInfo>& branch_info ///< [in] ブランチの情報
  );

  /// @brief set() の後処理
  void
  post_op();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief PPI系のノードの生成と登録
  NodeRep*
  new_ppi(
    const std::string& name, ///< [in] 名前
    std::function<NodeRep*(SizeType id,
			   SizeType input_id)> new_func ///< [in] ノードを生成する関数
  );

  /// @brief PPO系のノードの生成と登録
  NodeRep*
  new_ppo(
    const std::string& name, ///< [in] 名前
    std::function<NodeRep*(SizeType id,
			   SizeType output_id)> new_func ///< [in] ノードを生成する関数
  );

  /// @brief ノードを生成して登録する．
  NodeRep*
  new_node(
    std::function<NodeRep*(SizeType id)> new_func ///< [in] ノードを生成する関数
  );

  /// @brief ゲートタイプを生成して登録する．
  GateType*
  new_gate_type(
    std::function<GateType*(SizeType id)> new_func ///< [in] ゲートタイプを生成する関数
  );

  /// @brief ゲートを生成して登録する．
  GateRep*
  new_gate(
    std::function<GateRep*(SizeType id)> new_func ///< [in] ゲートを生成する関数
  );

  /// @brief FFR を作る．
  const FFRRep*
  new_ffr(
    const NodeRep* root ///< [in] FFR の根のノード
  );

  /// @brief MFFC を作る．
  void
  new_mffc(
    const NodeRep* root, ///< [in] root MFFCの根のノード
    const std::unordered_map<SizeType, const FFRRep*>& ffr_map ///< [in] ノード番号をキーにしてFFRを格納する辞書
  );

  /// @brief ゲートに関連した故障を作る．
  void
  gen_gate_faults(
    const GateRep* gate,            ///< [in] 対象のゲート
    std::vector<SizeType>& fault_map ///< [out] 各ノードの出力の故障を記録する配列
  );

  /// @brief ステムの故障を作る．
  void
  gen_stem_fault(
    const GateRep* gate,            ///< [in] 対象のゲート
    std::vector<SizeType>& fault_map ///< [out] 各ノードの出力の故障を記録する配列
  );

  /// @brief ブランチの故障を作る．
  void
  gen_branch_fault(
    const GateRep* gate ///< [in] 対象のゲート
  );

  /// @brief ゲート網羅故障を作る．
  void
  gen_ex_fault(
    const GateRep* gate  ///< [in] 対象のゲート
  );

  /// @brief 故障を生成して登録する．
  FaultRep*
  reg_fault(
    std::function<FaultRep*(SizeType fid)> new_fault ///< [in] 故障を生成する関数
  );

  /// @brief 代表故障マップを作る．
  void
  gen_rep_map(
    const GateRep* gate,                   ///< [in] 対象のゲート
    const std::vector<SizeType>& fault_map, ///< [in] 各ノードの出力の故障番号を記録した配列
    std::vector<SizeType>& rep_map          ///< [out] 代表故障番号を記録する配列
  );

  /// @brief ノード番号の範囲チェック
  void
  _check_node_id(
    SizeType id
  ) const
  {
    if ( id >= node_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief 入力番号の範囲チェック
  void
  _check_input_id(
    SizeType id
  ) const
  {
    if ( id >= input_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief 出力番号の範囲チェック
  void
  _check_output_id(
    SizeType id
  ) const
  {
    if ( id >= output_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief PPI番号の範囲チェック
  void
  _check_ppi_id(
    SizeType id
  ) const
  {
    if ( id >= ppi_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief PPO番号の範囲チェック
  void
  _check_ppo_id(
    SizeType id
  ) const
  {
    if ( id >= ppo_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief MFFC番号の範囲チェック
  void
  _check_mffc_id(
    SizeType id
  ) const
  {
    if ( id >= mffc_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief FFR番号の範囲チェック
  void
  _check_ffr_id(
    SizeType id
  ) const
  {
    if ( id >= ffr_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief ゲートタイプ番号の範囲チェック
  void
  _check_gate_type_id(
    SizeType id
  ) const
  {
    if ( id >= mGateTypeArray.size() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief ゲート番号の範囲チェック
  void
  _check_gate_id(
    SizeType id
  ) const
  {
    if ( id >= gate_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief DFF番号の範囲チェック
  void
  _check_dff_id(
    SizeType id
  ) const
  {
    if ( id >= dff_num() ) {
      throw std::out_of_range{"id is out of range"};
    }
  }

  /// @brief 故障番号の範囲チェック
  void
  _check_fault_id(
    SizeType fault_id
  ) const
  {
    if ( fault_id >= mFaultArray.size() ) {
      throw std::out_of_range{"fault_id is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力用の GateType
  const GateType* mInputGateType;

  // 出力用の GateType
  const GateType* mOutputGateType;

  // ゲートの情報を管理するオブジェクトの配列
  std::vector<std::unique_ptr<GateType>> mGateTypeArray;

  // ノードのポインタ配列
  std::vector<std::unique_ptr<NodeRep>> mNodeArray;

  // ゲートに関する情報の配列
  std::vector<std::unique_ptr<GateRep>> mGateArray;

  // PPIノードの配列
  std::vector<const NodeRep*> mPPIArray;

  // PPIの名前の配列
  std::vector<std::string> mPPINameArray;

  // PPOノードの配列
  std::vector<const NodeRep*> mPPOArray;

  // PPOの名前の配列
  std::vector<std::string> mPPONameArray;

  // TFI サイズの降順に整列したPPOノードの配列
  std::vector<const NodeRep*> mPPOArray2;

  // DFFの入力ノードと出力ノードのペアのリスト
  std::vector<std::pair<NodeRep*, NodeRep*>> mDffList;

  // MFFC の配列
  std::vector<std::unique_ptr<MFFCRep>> mMFFCArray;

  // MFFC の根のノード番号をキーにして MFFCを格納する辞書
  std::unordered_map<SizeType, const MFFCRep*> mMFFCMap;

  // FFR の配列
  std::vector<std::unique_ptr<FFRRep>> mFFRArray;

  // FFR の根のノード番号をキーにして FFR を格納する辞書
  std::unordered_map<SizeType, const FFRRep*> mFFRMap;

  // 故障の種類
  FaultType mFaultType{FaultType::None};

  // 故障の配列
  std::vector<std::unique_ptr<FaultRep>> mFaultArray;

  // 代表故障の故障番号のリスト
  std::vector<SizeType> mRepFidList;

};

END_NAMESPACE_DRUID

#endif // NETWORK_REP_H
