#ifndef TPGNODEFACTORY_H
#define TPGNODEFACTORY_H

/// @file TpgNodeFactory.h
/// @brief TpgNodeFactory のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgNodeFactory TpgNodeFactory.h "TpgNodeFactory.h"
/// @brief TpgNode(の派生クラス)を作るファクトリクラス
///
/// このクラスの機能を TpgNetwork に組み込んでも良いが，これだけ切り離したほうが
/// 見通しがよくなるのと TpgNode 単体のテストプログラムが書きやすくなる．
/// また，TpgNetwork.cc 中で TpgNode の各継承クラスのヘッダファイルを
/// インクルードする必要がなくなる．
//////////////////////////////////////////////////////////////////////
class TpgNodeFactory
{
public:

  /// @brief コンストラクタ
  TpgNodeFactory() = default;

  /// @brief デストラクタ
  ~TpgNodeFactory() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードを作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_input(
    int id,        ///< [in] ノード番号
    int iid,       ///< [in] 入力番号
    int fanout_num ///< [in] ファンアウト数
  );

  /// @brief 出力ノードを作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_output(
    int id,        ///< [in] ノード番号
    int oid,       ///< [in] 出力番号
    TpgNode* inode ///< [in] 入力ノード
  );

  /// @brief DFFの入力ノードを作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_input(
    int id,            ///< [in] ノード番号
    int oid,           ///< [in] 出力番号
    const TpgDff* dff, ///< [in] 接続しているDFF
    TpgNode* inode     ///< [in] 入力ノード
  );

  /// @brief DFFの出力ノードを作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_output(
    int id,            ///< [in] ノード番号
    int iid,           ///< [in] 入力番号
    const TpgDff* dff, ///< [in] 接続しているDFF
    int fanout_num     ///< [in] ファンアウト数
  );

  /// @brief DFFのクロック端子を作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_clock(
    int id,            ///< [in] ノード番号
    const TpgDff* dff, ///< [in] 接続しているDFF
    TpgNode* inode     ///< [in] 入力ノード
  );

  /// @brief DFFのクリア端子を作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_clear(
    int id,            ///< [in] ノード番号
    const TpgDff* dff, ///< [in] 接続しているDFF
    TpgNode* inode     ///< [in] 入力ノード
  );

  /// @brief DFFのプリセット端子を作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_dff_preset(
    int id,            ///< [in] ノード番号
    const TpgDff* dff, ///< [in] 接続しているDFF
    TpgNode* inode     ///< [in] 入力ノード
  );

  /// @brief 論理ノードを作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_logic(
    int id,                             ///< [in] ノード番号
    GateType gate_type,                 ///< [in] ゲートタイプ
    const vector<TpgNode*>& inode_list, ///< [in] 入力ノードのリスト
    int fanout_num                      ///< [in] ファンアウト数
  );

};

END_NAMESPACE_DRUID

#endif // TPGNODEFACTORY_H
