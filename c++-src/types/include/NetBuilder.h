#ifndef NETBUILDER_H
#define NETBUILDER_H

/// @file NetBuilder.h
/// @brief NetBuilder のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/BnModel.h"
#include "ym/PrimType.h"
#include "types/TpgNetwork.h"
#include "types/FaultType.h"
#include "NetworkRep.h"
#include "NodeRep.h"
#include "GateRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NetBuilder NetBuilder.h "NetBuilder.h"
/// @brief NetworkRep のビルダクラス
//////////////////////////////////////////////////////////////////////
class NetBuilder
{
public:

  /// @brief コンストラクタ
  NetBuilder(
    FaultType fault_type ///< [in] 故障の種類
  ) : mFaultType{fault_type}
  {
  }

  /// @brief デストラクタ
  ~NetBuilder() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  ///
  /// - 既に設定されている内容は破棄される．
  /// - 適切なサイズを指定するとメモリ再確保のオーバーヘッドを避けることができる．
  void
  init(
    SizeType input_num = 0,     ///< [in] 入力数
    SizeType output_num = 0,    ///< [in] 出力数
    SizeType dff_num = 0,       ///< [in] DFF数
    SizeType gate_num = 0,      ///< [in] ゲート数
    SizeType extra_node_num = 0 ///< [in] 追加のノード数
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
    const std::string& name,  ///< [in] ノード名
    const NodeRep* inode      ///< [in] 入力のノード
  );

  /// @brief DFFの入力ノードを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_dff_input_node(
    SizeType dff_id,         ///< [in] DFF番号
    const std::string& name, ///< [in] ノード名
    const NodeRep* inode     ///< [in] 入力のノード
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

  /// @brief GateType に対応する GateRep を作る．
  GateRep*
  make_gate(
    const GateType* gate_type,                    ///< [in] ゲートの種類
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief 終了処理を行って結果の TpgNetwork を返す．
  ///
  /// このオブジェクトはクリアされる．
  TpgNetwork
  wrap_up();

  /// @brief BnModel を TpgNetwork に変換する．
  static
  TpgNetwork
  from_bn(
    const BnModel& model, ///< [in] 返還元のネットワーク
    FaultType fault_type  ///< [in] 故障の種類
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief バッファを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_buff_node(
    const NodeRep* fanin ///< [in] ファンインのノード
  )
  {
    return make_prim_node(PrimType::Buff, {fanin});
  }

  /// @brief インバーターを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_not_node(
    const NodeRep* fanin ///< [in] ファンインのノード
  )
  {
    return make_prim_node(PrimType::Not, {fanin});
  }

  /// @brief 組み込み型の論理ゲートを生成する．
  /// @return 生成したノードを返す．
  NodeRep*
  make_prim_node(
    PrimType type,		                  ///< [in] ゲートの型
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  );

  /// @brief 論理式から TpgNode の木を生成する．
  ///
  /// leaf_nodes は 変数番号 * 2 + (0/1) に
  /// 該当する変数の肯定/否定のリテラルが入っている．
  NodeRep*
  make_cplx_node(
    const Expr& expr,	                           ///< [in] 式
    const std::vector<const NodeRep*>& leaf_nodes, ///< [in] 式のリテラルに対応するノードの配列
    std::vector<GateRep::BranchInfo>& branch_info  ///< [out] ブランチの情報
  );

  /// @brief ファンアウトの接続を追加する．
  void
  add_fanout(
    const NodeRep* node,
    const NodeRep* fo_node
  );

  /// @brief mNetwork が空なら生成する．
  void
  _check_network();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障の種類
  FaultType mFaultType;

  // 対象のネットワーク
  std::shared_ptr<NetworkRep> mNetwork;

  // ノード番号をキーとしてファンアウトの接続を保持する辞書
  std::unordered_map<SizeType, std::vector<const NodeRep*>> mConnectionList;

};

END_NAMESPACE_DRUID

#endif // NETBUILDER_H
