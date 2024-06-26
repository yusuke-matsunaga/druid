#ifndef TPGNODESET_H
#define TPGNODESET_H

/// @file TpgNodeSet.h
/// @brief TpgNodeSet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgNodeSet TpgNodeSet.h "TpgNodeSet.h"
/// @brief 重複のないノードのリストを作るクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class TpgNodeSet
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief TFO のノードを求める．
  static
  vector<const TpgNode*>
  get_tfo_list(
    SizeType max_size,             ///< [in] ノード番号の最大値 + 1
    const TpgNode* root,           ///< [in] 起点となるノード
    const TpgNode* bnode = nullptr ///< [in] ブロックノード
  );

  /// @brief TFO のノードを求める．
  static
  vector<const TpgNode*>
  get_tfo_list(
    SizeType max_size,                     ///< [in] ノード番号の最大値 + 1
    const TpgNode* root,                   ///< [in] 起点となるノード
    std::function<void(const TpgNode*)> op ///< [in] ノードに対するファンクタ
  );

  /// @brief TFO のノードを求める．
  static
  vector<const TpgNode*>
  get_tfo_list(
    SizeType max_size,                       ///< [in] ノード番号の最大値 + 1
    const vector<const TpgNode*>& root_list, ///< [in] 起点となるノードのリスト
    std::function<void(const TpgNode*)> op   ///< [in] ノードに対するファンクタ
  );

  /// @brief TFI のノードを求める．
  static
  vector<const TpgNode*>
  get_tfi_list(
    SizeType max_size,                      ///< [in] ノード番号の最大値 + 1
    const vector<const TpgNode*>& root_list ///< [in] 起点となるノード
  );

  /// @brief TFI のノードを求める．
  static
  vector<const TpgNode*>
  get_tfi_list(
    SizeType max_size,                       ///< [in] ノード番号の最大値 + 1
    const vector<const TpgNode*>& root_list, ///< [in] 起点となるノード
    std::function<void(const TpgNode*)> op   ///< [in] ノードに対するファンクタ
  );

  /// @brief 出力からの DFS を行う．
  static
  void
  dfs(
    SizeType max_size,                            ///< [in] ノード番号の最大値 + 1
    const vector<const TpgNode*>& root_list,      ///< [in] 起点となるノード
    std::function<void(const TpgNode*)> pre_func, ///< [in] pre-order の処理関数
    std::function<void(const TpgNode*)> post_func ///< [in] post-order の処理関数
  );

  /// @brief 出力からの DFS を行う．
  static
  void
  dfs_pre(
    SizeType max_size,                           ///< [in] ノード番号の最大値 + 1
    const vector<const TpgNode*>& root_list,     ///< [in] 起点となるノード
    std::function<void(const TpgNode*)> pre_func ///< [in] pre-order の処理関数
  )
  {
    dfs(max_size, root_list, pre_func, [](const TpgNode*){});
  }

  /// @brief 出力からの DFS を行う．
  static
  void
  dfs_post(
    SizeType max_size,                            ///< [in] ノード番号の最大値 + 1
    const vector<const TpgNode*>& root_list,      ///< [in] 起点となるノード
    std::function<void(const TpgNode*)> post_func ///< [in] post-order の処理関数
  )
  {
    dfs(max_size, root_list, [](const TpgNode*){}, post_func);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief マークをつける．
  ///
  /// と同時に queue に入れる．<br>
  /// すでにマークされていたら何もしない．
  static
  void
  set_mark(
    const TpgNode* node,               ///< [in] 対象のノード
    std::deque<const TpgNode*>& queue, ///< [in] キュー
    vector<bool>& mark_array           ///< [in] 処理済みの印を入れた配列
  )
  {
    SizeType id = node->id();
    if ( !mark_array[id] ) {
      mark_array[id] = true;
      queue.push_back(node);
    }
  }

};

END_NAMESPACE_DRUID

#endif // TPGNODESET_H
