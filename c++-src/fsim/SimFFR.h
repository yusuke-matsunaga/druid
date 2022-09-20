#ifndef SIMFFR_H
#define SIMFFR_H

/// @file SimFFR.h
/// @brief SimFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimFault;
class SimNode;

//////////////////////////////////////////////////////////////////////
/// @class SimFFR SimFFR.h "SimFFR.h"
/// @brief Fanout-Free-Region を表すクラス
//////////////////////////////////////////////////////////////////////
class SimFFR
{
public:

  /// @brief コンストラクタ
  SimFFR() = default;

  /// @brief コピーコンストラクタは禁止
  SimFFR(const SimFFR& src) = delete;

  /// @brief 代入演算子も禁止
  const SimFFR&
  operator=(const SimFFR& src) = delete;

  /// @brief デストラクタ
  ~SimFFR() = default;


public:

  /// @brief 根のノードをセットする．
  void
  set_root(
    SimNode* root
  )
  {
    mRoot = root;
  }

  /// @brief 根のノードを得る．
  SimNode*
  root() const
  {
    return mRoot;
  }

  /// @brief このFFRの故障リストに故障を追加する．
  void
  add_fault(
    SimFault* f
  )
  {
    mFaultList.push_back(f);
  }

  /// @brief このFFRに属する故障リストを得る．
  const vector<SimFault*>&
  fault_list() const
  {
    return mFaultList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  SimNode* mRoot{nullptr};

  // この FFR に属する故障のリスト
  vector<SimFault*> mFaultList;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMFFR_H
