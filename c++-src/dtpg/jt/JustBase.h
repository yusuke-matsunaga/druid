#ifndef JUSTBASE_H
#define JUSTBASE_H

/// @file JustBase.h
/// @brief JustBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Justifier.h"
#include "types/TpgNode.h"
#include "types/Assign.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_DRUID

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class JustBase JustBase.h "JustBase.h"
/// @brief JustImpl の実装クラス
///
/// いくつかの仮想関数が未定義なので継承クラスで実装する必要がある．
//////////////////////////////////////////////////////////////////////
class JustBase :
  public Justifier
{
public:

  /// @brief コンストラクタ
  JustBase(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  virtual
  ~JustBase();


private:
  //////////////////////////////////////////////////////////////////////
  // JustImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief justify の実際の処理
  AssignList
  _justify(
    const AssignList& assign_list ///< [in] 割当リスト
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化処理
  virtual
  void
  just_init(
    const AssignList& assign_list ///< [in] 割当リスト
  ) = 0;

  /// @brief 正当化処理
  void
  just_main(
    const TpgNode& node,       ///< [in] 対象のノード
    int time,		       ///< [in] 時刻 ( 0 or 1 )
    AssignList& pi_assign_list ///< [in] 結果の割当を保持するリスト
  );

  /// @brief 制御値を持つファンインを一つ選ぶ．
  /// @return 選んだファンインのノードを返す．
  virtual
  TpgNode
  select_cval_node(
    const TpgNode& node, ///< [in] 対象のノード
    int time		 ///< [in] 時刻 ( 0 or 1 )
  ) = 0;

  /// @brief 終了処理
  virtual
  void
  just_end() = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // マークに関するアクセス関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューに追加する
  void
  put_queue(
    const TpgNode& node, ///< [in] 対象のノード
    int time		 ///< [in] タイムフレーム ( 0 or 1 )
  )
  {
    if ( !mark(node, time) ) {
      set_mark(node, time);
      mQueue.push_back({node, time});
    }
  }

  /// @brief justified マークをつけ，mJustifiedNodeList に加える．
  void
  set_mark(
    const TpgNode& node, ///< [in] 対象のノード
    int time		 ///< [in] タイムフレーム ( 0 or 1 )
  )
  {
    // 念のため time の最下位ビットだけ使う．
    time &= 1;
    mMarkArray[node.id()][time] = true;
  }

  /// @brief justified マークを読む．
  bool
  mark(
    const TpgNode& node, ///< [in] 対象のノード
    int time		 ///< [in] タイムフレーム ( 0 or 1 )
  ) const
  {
    // 念のため time の最下位ビットだけ使う．
    time &= 1;
    return mMarkArray[node.id()][time];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 個々のノードのマークを表す配列
  std::vector<std::bitset<2>> mMarkArray;

  // 対象のノードを入れるキュー
  std::vector<std::pair<TpgNode, int>> mQueue;

};

END_NAMESPACE_DRUID

#endif // JUSTBASE_H
