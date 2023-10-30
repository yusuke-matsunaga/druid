#ifndef TPGNETWORK_H
#define TPGNETWORK_H

/// @file TpgNetwork.h
/// @brief TpgNetwork のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/bn.h"
#include "ym/ClibCellLibrary.h"
#include "TpgDFFList.h"
#include "TpgMFFCList.h"
#include "TpgFFRList.h"
#include "TpgGateList.h"


BEGIN_NAMESPACE_DRUID

class TpgNetworkImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgNetwork TpgNetwork.h "TpgNetwork.h"
/// @brief DRUID 用のネットワークを表すクラス
/// @sa TpgNode
/// @sa TpgDff
///
/// 基本的には TpgNode のネットワーク(DAG)を表す．
///
/// ただし，順序回路を扱うために TpgDff というクラスを持つ．
/// TpgDff の入出力はそれぞれ疑似出力，疑似入力の TpgNode を持つ．
///
/// 本当の入力と疑似入力をあわせて PPI(Pseudo Primary Input) と呼ぶ．
///
/// 本当の出力と疑似出力をあわせて PPO(Pseudo Primary Output) と呼ぶ．
///
/// DFFのクロック系の回路は無視される．
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
//////////////////////////////////////////////////////////////////////
class TpgNetwork
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgNetwork();

  /// @brief コピーコンストラクタは禁止
  TpgNetwork(
    const TpgNetwork& src
  ) = delete;

  /// @brief コピー代入演算子も禁止
  TpgNetwork&
  operator=(
    const TpgNetwork& src
  ) = delete;

  /// @brief ムーブコンストラクタ
  TpgNetwork(
    TpgNetwork&& src ///< [in] ムーブ元
  );

  /// @brief ムーブ代入演算子
  TpgNetwork&
  operator=(
    TpgNetwork&& src ///< [in] ムーブ元
  );

  /// @brief BnModel からの変換コンストラクタ
  TpgNetwork(
    const BnModel& src ///< [in] 設定元のネットワーク
  );

  /// @brief blif ファイルを読み込む．
  ///
  /// エラーが起こったら std::invalid_argument 例外を送出する．
  static
  TpgNetwork
  read_blif(
    const string& filename,        ///< [in] ファイル名
    const string& clock_name = {}, ///< [in] クロック入力名
    const string& reset_name = {}  ///< [in] リセット入力名
  )
  {
    return read_blif(filename, ClibCellLibrary{}, clock_name, reset_name);
  }

  /// @brief blif ファイルを読み込む．
  ///
  /// エラーが起こったら std::invalid_argument 例外を送出する．
  static
  TpgNetwork
  read_blif(
    const string& filename,              ///< [in] ファイル名
    const ClibCellLibrary& cell_library, ///< [in] セルライブラリ
    const string& clock_name = {},       ///< [in] クロック入力名
    const string& reset_name = {}        ///< [in] リセット入力名
  );

  /// @brief iscas89 形式のファイルを読み込む．
  ///
  /// エラーが起こったら std::invalid_argument 例外を送出する．
  static
  TpgNetwork
  read_iscas89(
    const string& filename,       ///< [in] ファイル名
    const string& clock_name = {} ///< [in] クロック入力名
  );

  /// @brief ファイルを読み込む
  static
  TpgNetwork
  read_network(
    const string& filename,                   ///< [in] ファイル名
    const string& format,                     ///< [in] ファイルの形式を表す文字列
    const ClibCellLibrary& cell_library = {}, ///< [in] セルライブラリ
    const string& clock_name = {},            ///< [in] クロック入力名
    const string& reset_name = {}             ///< [in] リセット入力名
  );

  /// @brief デストラクタ
  ~TpgNetwork();


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
  /// node = network.node(node->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  node(
    SizeType id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const;

  /// @brief 全ノードのリストを得る．
  const vector<const TpgNode*>&
  node_list() const;

  /// @brief 外部入力数を得る．
  SizeType
  input_num() const;

  /// @brief 外部入力ノードを得る．
  ///
  /// @code
  /// node = network.input(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  input(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < input_num() )
  ) const;

  /// @brief 外部入力ノードのリストを得る．
  const vector<const TpgNode*>&
  input_list() const;

  /// @brief 外部出力数を得る．
  SizeType
  output_num() const;

  /// @brief 外部出力ノードを得る．
  ///
  /// @code
  /// node = network.output(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const;

  /// @brief 外部出力ノードのリストを得る．
  const vector<const TpgNode*>&
  output_list() const;

  /// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
  ///
  /// @code
  /// node = network.output2(node->output_id2())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
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
  /// node = network.ppi(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppi(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ppi_num() )
  ) const;

  /// @brief PPI の名前を返す．
  string
  ppi_name(
    SizeType input_id ///< [in] 入力番号
  ) const;

  /// @brief 擬似外部入力のリストを得る．
  const vector<const TpgNode*>&
  ppi_list() const;

  /// @brief スキャン方式の擬似外部出力数を得る．
  ///
  /// = output_num() + dff_num()
  SizeType
  ppo_num() const;

  /// @brief スキャン方式の擬似外部出力を得る．
  ///
  /// @code
  /// node = network.ppo(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppo(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ppo_num() )
  ) const;

  /// @brief PPO の名前を返す．
  string
  ppo_name(
    SizeType output_id ///< [in] 出力番号
  ) const;

  /// @brief 擬似外部出力のリストを得る．
  const vector<const TpgNode*>&
  ppo_list() const;

  /// @brief MFFC 数を返す．
  SizeType
  mffc_num() const;

  /// @brief MFFC を返す．
  TpgMFFC
  mffc(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < mffc_num() )
  ) const;

  /// @brief MFFC のリストを得る．
  TpgMFFCList
  mffc_list() const
  {
    return TpgMFFCList{mImpl.get(), mffc_num()};
  }

  /// @brief FFR 数を返す．
  SizeType
  ffr_num() const;

  /// @brief FFR を返す．
  TpgFFR
  ffr(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const;

  /// @brief FFR のリストを得る．
  TpgFFRList
  ffr_list() const
  {
    return TpgFFRList{mImpl.get(), ffr_num()};
  }

  /// @brief DFF数を得る．
  SizeType
  dff_num() const;

  /// @brief DFF を得る．
  ///
  /// @code
  /// dff = network.dff(dff->id())
  /// @endcode
  /// の関係が成り立つ．
  TpgDFF
  dff(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const;

  /// @brief DFF のリストを得る．
  TpgDFFList
  dff_list() const;


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
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < gate_num() )
  ) const;

  /// @brief ゲート情報のリストを得る．
  TpgGateList
  gate_list() const
  {
    return TpgGateList{mImpl.get(), gate_num()};
  }


public:
  //////////////////////////////////////////////////////////////////////
  // その他
  //////////////////////////////////////////////////////////////////////

  /// @brief TpgNetwork の内容を出力する関数(デバッグ用)
  void
  print(
    ostream& s ///< [in] 出力先のストリーム
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgNetwork の実装
  std::unique_ptr<TpgNetworkImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // TPGNETWORK_H
