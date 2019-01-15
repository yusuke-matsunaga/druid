#ifndef TPGNETWORK_H
#define TPGNETWORK_H

/// @file TpgNetwork.h
/// @brief TpgNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/Array.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgNetwork TpgNetwork.h "TpgNetwork.h"
/// @brief DRUID 用のネットワークを表すクラス
/// @sa TpgNode
/// @sa TpgDff
/// @sa TpgFault
///
/// 基本的には TpgNode のネットワーク(DAG)を表す．
/// ただし，順序回路を扱うために TpgDff というクラスを持つ．
/// TpgDff の入出力はそれぞれ疑似出力，疑似入力の TpgNode を持つ．<br>
/// 本当の入力と疑似入力をあわせて PPI(Pseudo Primary Input) と呼ぶ．<br>
/// 本当の出力と疑似出力をあわせて PPO(Pseudo Primary Output) と呼ぶ．<br>
/// クロック系の回路の情報も保持されるが，一般のノードとは区別される．
/// セット/リセット系の回路は通常の論理系の回路とみなす．
/// このクラスは const BnNetwork& から設定され，以降，一切変更されない．
/// 設定用の便利関数として blif フォーマットと isca89(.bench) フォーマットの
/// ファイルを読み込んで内容を設定する関数もある．<br>
/// 内容が設定されると同時に故障も定義される．
/// 構造的に等価な故障の中で一つ代表故障を決めて代表故障のリストを作る．
/// 代表故障はネットワーク全体，FFR，ノードごとにリスト化される．<br>
//////////////////////////////////////////////////////////////////////
class TpgNetwork
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  TpgNetwork();

  /// @brief コピーコンストラクタは禁止
  TpgNetwork(const TpgNetwork& src) = delete;

  /// @brief コピー代入演算子も禁止
  TpgNetwork&
  operator=(const TpgNetwork& src) = delete;

  /// @brief ムーブコンストラクタは禁止
  TpgNetwork(TpgNetwork&& src) = delete;

  /// @brief ムーブ代入演算子も禁止
  TpgNetwork&
  operator=(TpgNetwork&& src) = delete;

  /// @brief デストラクタ
  ~TpgNetwork();


public:
  //////////////////////////////////////////////////////////////////////
  // 通常の構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を得る．
  int
  node_num() const;

  /// @brief ノードを得る．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  ///
  /// @code
  /// node = network.node(node->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  node(int id) const;

  /// @brief 全ノードのリストを得る．
  Array<const TpgNode*>
  node_list() const;

  /// @brief ノード名を得る．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  const char*
  node_name(int id) const;

  /// @brief 外部入力数を得る．
  int
  input_num() const;

  /// @brief 外部入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  ///
  /// @code
  /// node = network.input(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  input(int pos) const;

  /// @brief 外部入力ノードのリストを得る．
  Array<const TpgNode*>
  input_list() const;

  /// @brief 外部出力数を得る．
  int
  output_num() const;

  /// @brief 外部出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  ///
  /// @code
  /// node = network.output(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output(int pos) const;

  /// @brief 外部出力ノードのリストを得る．
  Array<const TpgNode*>
  output_list() const;

  /// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  ///
  /// @code
  /// node = network.output2(node->output_id2())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output2(int pos) const;

  /// @brief スキャン方式の擬似外部入力数を得る．
  ///
  /// = input_num() + dff_num()
  int
  ppi_num() const;

  /// @brief スキャン方式の擬似外部入力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ppi_num() )
  ///
  /// @code
  /// node = network.ppi(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppi(int pos) const;

  /// @brief 擬似外部入力のリストを得る．
  Array<const TpgNode*>
  ppi_list() const;

  /// @brief スキャン方式の擬似外部出力数を得る．
  ///
  /// = output_num() + dff_num()
  int
  ppo_num() const;

  /// @brief スキャン方式の擬似外部出力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ppo_num() )
  ///
  /// @code
  /// node = network.ppo(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppo(int pos) const;

  /// @brief 擬似外部出力のリストを得る．
  Array<const TpgNode*>
  ppo_list() const;

  /// @brief MFFC 数を返す．
  int
  mffc_num() const;

  /// @brief MFFC を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < mffc_num() )
  const TpgMFFC&
  mffc(int pos) const;

  /// @brief MFFC のリストを得る．
  Array<const TpgMFFC>
  mffc_list() const;

  /// @brief FFR 数を返す．
  int
  ffr_num() const;

  /// @brief FFR を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
  const TpgFFR&
  ffr(int pos) const;

  /// @brief FFR のリストを得る．
  Array<const TpgFFR>
  ffr_list() const;

  /// @brief DFF数を得る．
  int
  dff_num() const;

  /// @brief DFF を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
  ///
  /// @code
  /// dff = network.dff(dff->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgDff&
  dff(int pos) const;

  /// @brief DFF のリストを得る．
  Array<const TpgDff>
  dff_list() const;

  /// @brief 故障IDの最大値+1を返す．
  int
  max_fault_id() const;

  /// @brief 全代表故障数を返す．
  int
  rep_fault_num() const;

  /// @brief 代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < rep_fault_num() )
  const TpgFault*
  rep_fault(int pos) const;

  /// @brief 代表故障のリストを返す．
  Array<const TpgFault*>
  rep_fault_list() const;

  /// @brief ノードに関係した代表故障数を返す．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  int
  node_rep_fault_num(int id) const;

  /// @brief ノードに関係した代表故障を返す．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  /// @param[in] pos 位置番号 ( 0 <= pos < node_rep_fault_num(id) )
  const TpgFault*
  node_rep_fault(int id,
		 int pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定するための関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork から内容を設定する．
  /// @param[in] network 設定元のネットワーク
  void
  set(const BnNetwork& network);

  /// @brief blif ファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @return 読み込みが成功したら true を返す．
  bool
  read_blif(const string& filename);

  /// @brief blif ファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @param[in] cell_library セルライブラリ
  /// @return 読み込みが成功したら true を返す．
  bool
  read_blif(const string& filename,
	    const ClibCellLibrary& cell_library);

  /// @brief iscas89 形式のファイルを読み込む．
  /// @param[in] filename ファイル名
  /// @return 読み込みが成功したら true を返す．
  bool
  read_iscas89(const string& filename);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgNetwork の実装
  std::unique_ptr<TpgNetworkImpl> mImpl;

};

/// @brief TpgNetwork の内容を出力する関数
/// @param[in] s 出力先のストリーム
/// @param[in] network 対象のネットワーク
void
print_network(ostream& s,
	      const TpgNetwork& network);

END_NAMESPACE_DRUID

#endif // TPGNETWORK_H
