#ifndef SIMFFR_H
#define SIMFFR_H

/// @file SimFFR.h
/// @brief SimFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
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
  SimFFR();

  /// @brief コピーコンストラクタは禁止
  SimFFR(const SimFFR& src) = delete;

  /// @brief 代入演算子も禁止
  const SimFFR&
  operator=(const SimFFR& src) = delete;

  /// @brief デストラクタ
  ~SimFFR();


public:

  /// @brief 根のノードをセットする．
  void
  set_root(SimNode* root);

  /// @brief 根のノードを得る．
  SimNode*
  root() const;

  /// @brief このFFRの故障リストに故障を追加する．
  void
  add_fault(SimFault* f);

  /// @brief このFFRに属する故障リストを得る．
  const vector<SimFault*>&
  fault_list() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 根のノード
  SimNode* mRoot;

  // この FFR に属する故障のリスト
  vector<SimFault*> mFaultList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
SimFFR::SimFFR()
{
}

// @brief デストラクタ
inline
SimFFR::~SimFFR()
{
}

// @brief 根のノードをセットする．
inline
void
SimFFR::set_root(SimNode* root)
{
  mRoot = root;
}

// @brief 根のノードを得る．
inline
SimNode*
SimFFR::root() const
{
  return mRoot;
}

// @brief このFFRの故障リストに故障を追加する．
inline
void
SimFFR::add_fault(SimFault* f)
{
  mFaultList.push_back(f);
}

// @brief このFFRに属する故障リストを得る．
inline
const vector<SimFault*>&
SimFFR::fault_list() const
{
  return mFaultList;
}

END_NAMESPACE_DRUID_FSIM

#endif // SIMFFR_H
