#ifndef DFSDRIVER_H
#define DFSDRIVER_H

/// @file DfsDriver.h
/// @brief DfsDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class NodeRep;

//////////////////////////////////////////////////////////////////////
/// @class DfsDriver DfsDriver.h "DfsDriver.h"
/// @brief NodeRep の DFS を行うクラス
//////////////////////////////////////////////////////////////////////
class DfsDriver
{
  using CallBack = std::function<void(const NodeRep*)>;

public:

  /// @brief コンストラクタ
  DfsDriver(
    SizeType node_num, ///< [in] ノード数
    CallBack pre_func, ///< [in] pre-order で実行される関数
    CallBack post_func ///< [in] post-order で実行される関数
  ) : mMarkArray(node_num, false),
      mPreFunc{pre_func},
      mPostFunc{post_func}
  {
  }

  /// @brief デストラクタ
  ~DfsDriver() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dfs を行う．
  void
  operator()(
    const NodeRep* node_rep
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 処理済みの印
  std::vector<bool> mMarkArray;

  // pre-order で実行される関数
  CallBack mPreFunc;

  // post-order で実行される関数
  CallBack mPostFunc;

};

END_NAMESPACE_DRUID

#endif // DFSDRIVER_H
