#ifndef NODELIST_H
#define NODELIST_H

/// @file NodeList.h
/// @brief NodeList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeList NodeList.h "NodeList.h"
/// @brief 重複のないノードのリストを作るクラス
//////////////////////////////////////////////////////////////////////
class NodeList
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief TFO のノードを求める．
  static
  vector<const TpgNode*>
  get_tfo_list(
    SizeType max_size,  ///< [in] ノード番号の最大値 + 1
    const TpgNode* root ///< [in] 起点となるノード
  );

  /// @brief TFI のノードを求める．
  static
  vector<const TpgNode*>
  get_tfi_list(
    SizeType max_size,  ///< [in] ノード番号の最大値 + 1
    const vector<const TpgNode*>& root_list ///< [in] 起点となるノード
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief マークをつける．
  ///
  /// と同時に node_list に入れる．<br>
  /// すでにマークされていたら何もしない．
  static
  void
  set_mark(
    const TpgNode* node,               ///< [in] 対象のノード
    vector<const TpgNode*>& node_list, ///< [in] 結果のノードを格納するリスト
    vector<bool>& mark_array           ///< [in] 処理済みの印を入れた配列
  )
  {
    SizeType id = node->id();
    if ( !mark_array[id] ) {
      mark_array[id] = true;
      node_list.push_back(node);
    }
  }

};

END_NAMESPACE_DRUID

#endif // NODELIST_H
