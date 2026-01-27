#ifndef TPGNETWORK_H
#define TPGNETWORK_H

/// @file TpgNetwork.h
/// @brief TpgNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgBase.h"
#include "types/TpgNode.h"
#include "types/TpgNodeList.h"
#include "types/TpgFFR.h"
#include "types/TpgFFRList.h"
#include "types/TpgMFFC.h"
#include "types/TpgMFFCList.h"
#include "types/TpgGate.h"
#include "types/TpgGateList.h"
#include "ym/bn.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgNetwork TpgNetwork.h "TpgNetwork.h"
/// @brief DRUID 用のネットワークを表すクラス
/// @sa TpgNode
/// @sa TpgDff
///
/// 基本的には TpgNode のネットワーク(DAG)を表す．
///
/// ただし，フルスキャンの順序回路を扱うためにフリップフロップの
/// 入出力をそれぞれ疑似出力，疑似入力としてあつかう．
///
/// - 本当の入力と疑似入力をあわせて PPI(Pseudo Primary Input) と呼ぶ．
///
/// - 本当の出力と疑似出力をあわせて PPO(Pseudo Primary Output) と呼ぶ．
///
/// フリップフロップのクロック系の回路は無視される．
/// セット/リセット系の回路も無視される．
///
/// TpgNode は全て単純な型の論理ゲートを表す．
/// もとのゲートが複合ゲートの場合には複数のノードに分解される．
/// そこで，オリジナルのネットワーク中のゲートを「ゲート」，分解された
/// TpgNode を「ノード」と呼んで区別することにする．
///
/// 内容が設定されると同時に故障も定義される．
/// 構造的に等価な故障の中で一つ代表故障を決めて代表故障のリストを作る．
/// 代表故障はネットワーク全体，FFR，ノードごとにリスト化される．
///
/// TpgNetwork は値を変更するメソッド(メンバ関数)を持たない．
/// そのため，初期化時にすべての情報を設定する必要がある．
/// 現時点では BnModel から設定する方法と blif/iscas89 ファイルを
/// 読み込む方法がある．
//////////////////////////////////////////////////////////////////////
class TpgNetwork:
  public TpgBase
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgNetwork() = default;

  /// @biref NetworkRep を指定したコンストラクタ
  TpgNetwork(
    std::shared_ptr<NetworkRep>& network ///< [in] 本体
  ) : TpgBase(network)
  {
  }

  /// @brief BnModel から変換する．
  static
  TpgNetwork
  from_bn(
    const BnModel& src,  ///< [in] 設定元のネットワーク
    FaultType fault_type ///< [in] 故障の種類
  );

  /// @brief blif ファイルを読み込む．
  ///
  /// エラーが起こったら std::invalid_argument 例外を送出する．
  static
  TpgNetwork
  read_blif(
    const std::string& filename, ///< [in] ファイル名
    FaultType fault_type         ///< [in] 故障の種類
  );

  /// @brief iscas89 形式のファイルを読み込む．
  ///
  /// エラーが起こったら std::invalid_argument 例外を送出する．
  static
  TpgNetwork
  read_iscas89(
    const std::string& filename, ///< [in] ファイル名
    FaultType fault_type         ///< [in] 故障の種類
  );

  /// @brief ファイルを読み込む
  static
  TpgNetwork
  read_network(
    const std::string& filename, ///< [in] ファイル名
    const std::string& format,   ///< [in] ファイルの形式を表す文字列
    FaultType fault_type         ///< [in] 故障の種類
  );

  /// @brief デストラクタ
  ~TpgNetwork() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 通常の構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を得る．
  SizeType
  node_num() const;

  /// @brief ノードを得る．
  ///
  /// @code
  /// node = network.node(node.id())
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  node(
    SizeType id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const;

  /// @brief 全ノードのリストを得る．
  TpgNodeList
  node_list() const;

  /// @brief 外部入力数を得る．
  SizeType
  input_num() const;

  /// @brief 外部入力ノードを得る．
  ///
  /// @code
  /// node = network.input(node.input_id())
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  input(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < input_num() )
  ) const;

  /// @brief 外部入力ノードのリストを得る．
  TpgNodeList
  input_list() const;

  /// @brief 外部出力数を得る．
  SizeType
  output_num() const;

  /// @brief 外部出力ノードを得る．
  ///
  /// @code
  /// node = network.output(node.output_id())
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  output(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief 外部出力ノードのリストを得る．
  TpgNodeList
  output_list() const;

  /// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
  ///
  /// @code
  /// node = network.output2(node.output_id2())
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  output2(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief スキャン方式の擬似外部入力数を得る．
  ///
  /// = input_num() + dff_num()
  SizeType
  ppi_num() const;

  /// @brief スキャン方式の擬似外部入力を得る．
  ///
  /// @code
  /// node = network.ppi(node.input_id())
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  ppi(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ppi_num() )
  ) const;

  /// @brief PPI の名前を返す．
  std::string
  ppi_name(
    SizeType input_id ///< [in] 入力番号
  ) const;

  /// @brief 擬似外部入力のリストを得る．
  TpgNodeList
  ppi_list() const;

  /// @brief スキャン方式の擬似外部出力数を得る．
  ///
  /// = output_num() + dff_num()
  SizeType
  ppo_num() const;

  /// @brief スキャン方式の擬似外部出力を得る．
  ///
  /// @code
  /// node = network.ppo(node.output_id())
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  ppo(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ppo_num() )
  ) const;

  /// @brief PPO の名前を返す．
  std::string
  ppo_name(
    SizeType output_id ///< [in] 出力番号
  ) const;

  /// @brief 擬似外部出力のリストを得る．
  TpgNodeList
  ppo_list() const;

  /// @brief MFFC 数を返す．
  SizeType
  mffc_num() const;

  /// @brief MFFC を返す．
  TpgMFFC
  mffc(
    SizeType mffc_id ///< [in] MFFC番号 ( 0 <= mffc_id < mffc_num() )
  ) const;

  /// @brief node の属している MFFC を返す．
  TpgMFFC
  mffc(
    const TpgNode& node ///< [in] 対象のノード
  ) const;

  /// @brief fault の属している FFR を返す．
  TpgMFFC
  mffc(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief MFFC のリストを得る．
  TpgMFFCList
  mffc_list() const;

  /// @brief FFR 数を返す．
  SizeType
  ffr_num() const;

  /// @brief FFR を返す．
  TpgFFR
  ffr(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const;

  /// @brief node の属している FFR を返す．
  TpgFFR
  ffr(
    const TpgNode& node ///< [in] 対象のノード
  ) const;

  /// @brief fault の属している FFR を返す．
  TpgFFR
  ffr(
    const TpgFault& fault ///< [in] 対象の故障
  ) const;

  /// @brief FFR のリストを得る．
  TpgFFRList
  ffr_list() const;

  /// @brief DFF数を得る．
  SizeType
  dff_num() const;

  /// @brief DFFの入力ノードを得る．
  ///
  /// @code
  /// network.dff_input(dff_id).dff_id() == dff_id
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  dff_input(
    SizeType dff_id ///< [in] DFF番号 ( 0 <= dff_id < dff_num() )
  ) const;

  /// @brief DFFの出力ノードを得る．
  ///
  /// @code
  /// network.dff_output(dff_id).dff_id() == dff_id
  /// @endcode
  /// の関係が成り立つ．
  TpgNode
  dff_output(
    SizeType dff_id ///< [in] 位置番号 ( 0 <= dff_id < dff_num() )
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // TFO/TFI を求める関数
  //////////////////////////////////////////////////////////////////////

  /// @brief TFO のノードを求める．
  TpgNodeList
  get_tfo_list(
    const TpgNode& root,             ///< [in] 起点となるノード
    const TpgNode& block = TpgNode{} ///< [in] ブロックノード
  ) const;

  /// @brief TFO のノードを求める．
  TpgNodeList
  get_tfo_list(
    const TpgNode& root,                   ///< [in] 起点となるノード
    std::function<void(const TpgNode&)> op ///< [in] ノードに対するファンクタ
  ) const;

  /// @brief TFO のノードを求める．
  TpgNodeList
  get_tfo_list(
    const TpgNodeList& root_list,          ///< [in] 起点となるノードのリスト
    std::function<void(const TpgNode&)> op ///< [in] ノードに対するファンクタ
  ) const
  {
    return get_tfo_list(root_list, TpgNode{}, op);
  }

  /// @brief TFO のノードを求める．
  TpgNodeList
  get_tfo_list(
    const TpgNodeList& root_list,          ///< [in] 起点となるノードのリスト
    const TpgNode& block,                  ///< [in] ブロックノード
    std::function<void(const TpgNode&)> op ///< [in] ノードに対するファンクタ
  ) const;

  /// @brief TFI のノードを求める．
  TpgNodeList
  get_tfi_list(
    const TpgNodeList& root_list ///< [in] 起点となるノード
  ) const
  {
    return get_tfi_list(root_list, [](const TpgNode&){});
  }

  /// @brief TFI のノードを求める．
  TpgNodeList
  get_tfi_list(
    const TpgNodeList& root_list,          ///< [in] 起点となるノード
    std::function<void(const TpgNode&)> op ///< [in] ノードに対するファンクタ
  ) const;

  /// @brief 出力からの DFS を行う．
  void
  dfs(
    const TpgNodeList& root_list,                 ///< [in] 起点となるノード
    std::function<void(const TpgNode&)> pre_func, ///< [in] pre-order の処理関数
    std::function<void(const TpgNode&)> post_func ///< [in] post-order の処理関数
  ) const;

  /// @brief 出力からの DFS を行う．
  void
  dfs_pre(
    const TpgNodeList& root_list,                ///< [in] 起点となるノード
    std::function<void(const TpgNode&)> pre_func ///< [in] pre-order の処理関数
  ) const
  {
    dfs(root_list, pre_func, [](const TpgNode&){});
  }

  /// @brief 出力からの DFS を行う．
  void
  dfs_post(
    const TpgNodeList& root_list,                 ///< [in] 起点となるノード
    std::function<void(const TpgNode&)> post_func ///< [in] post-order の処理関数
  ) const
  {
    dfs(root_list, [](const TpgNode&){}, post_func);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // オリジナルのゲートに関する情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲート数を返す．
  SizeType
  gate_num() const;

  /// @brief ゲート情報を得る．
  TpgGate
  gate(
    SizeType gid ///< [in] ゲート番号 ( 0 <= gid < gate_num() )
  ) const;

  /// @brief ゲートのリストを返す．
  TpgGateList
  gate_list() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障の情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const;

  /// @brief 1時刻前の状態を持つ時 true を返す．
  bool
  has_prev_state() const;

  /// @brief 代表故障のリストを得る．
  TpgFaultList
  rep_fault_list() const;

  /// @brief 故障番号の最大値を返す．
  SizeType
  max_fault_id() const;

  /// @brief 故障を得る．
  TpgFault
  fault(
    SizeType fault_id ///< [in] 故障番号 ( 0 <= fault_id < max_fault_id() )
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // その他
  //////////////////////////////////////////////////////////////////////

  /// @brief TpgNetwork の内容を出力する関数(デバッグ用)
  void
  print(
    std::ostream& s ///< [in] 出力先のストリーム
  ) const;

};

END_NAMESPACE_DRUID

#endif // TPGNETWORK_H
