#ifndef REFSIM_H
#define REFSIM_H

/// @file RefSim.h
/// @brief RefSim のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "fsim/DiffBits.h"


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
    const TpgNetwork& network ///< [in] ネットワーク
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
    SizeType fid          ///< [in] 対象の故障番号
  );

  /// @brief 縮退故障用の故障シミュレーションを行う．
  DiffBits
  simulate_sa(
    const TestVector& tv, ///< [in] テストベクタ
    SizeType fid          ///< [in] 対象の故障番号
  );

  /// @brief 遷移故障用の故障シミュレーションを行う．
  DiffBits
  simulate_td(
    const TestVector& tv, ///< [in] テストベクタ
    SizeType fid          ///< [in] 対象の故障番号
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief TpgNode に対応する RefNode を作る．
  RefNode*
  make_node(
    const TpgNode& tpg_node ///< [in] 元のノード
  );

  /// @brief 故障の活性化条件をチェックする．
  bool
  check_fault_cond(
    SizeType fid,
    RefNode* node
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 故障の種類
  FaultType mFaultType;

  // TpgNode のノード番号をキーにして RefNode を格納する配列
  std::vector<RefNode*> mNodeMap;

  // 入力ノードのリスト
  std::vector<RefNode*> mInputList;

  // DFFの出力ノード(疑似入力ノード)のリスト
  std::vector<RefNode*> mDffOutList;

  // 論理ノードのリスト
  std::vector<RefNode*> mLogicList;

  // 出力ノードのリスト
  std::vector<RefNode*> mOutputList;

  // DFFの入力ノード(疑似出力ノード)のリスト
  std::vector<RefNode*> mDffInList;

};

END_NAMESPACE_DRUID

#endif // REFSIM_H
