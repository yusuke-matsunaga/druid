#ifndef REFSIM_H
#define REFSIM_H

/// @file RefSim.h
/// @brief RefSim のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class RefNode;

//////////////////////////////////////////////////////////////////////
/// @class RefSim RefSim.h "RefSim.h"
/// @brief 故障シミュレーターのテスト用のリファレンス実装
///
/// とにかくシンプルに作る．
//////////////////////////////////////////////////////////////////////
class RefSim
{
public:

  /// @brief コンストラクタ
  RefSim(
    const TpgNetwork& network, ///< [in] ネットワーク
    bool has_previous_state    ///< [in] 1時刻前の値を持つ時 true にする．
  );

  /// @brief デストラクタ
  ~RefSim();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションを行う．
  DiffBits
  simulate(
    const TestVector& tv, ///< [in] テストベクタ
    const TpgFault& fault ///< [in] 故障
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief TpgNode に対応する RefNode を作る．
  RefNode*
  make_node(
    const TpgNode* tpg_node ///< [in] 元のノード
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgNode のノード番号をキーにして RefNode を格納する配列
  vector<RefNode*> mNodeMap;

  // 入力ノードのリスト
  vector<RefNode*> mInputList;

  // DFFノードのリスト
  vector<RefNode*> mDffList;

  // 論理ノードのリスト
  vector<RefNode*> mLogicList;

  // 出力ノードのリスト
  vector<RefNode*> mOutputList;

};

END_NAMESPACE_DRUID

#endif // REFSIM_H
