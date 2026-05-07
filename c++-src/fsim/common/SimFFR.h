#ifndef SIMFFR_H
#define SIMFFR_H

/// @file SimFFR.h
/// @brief SimFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
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

  /// @brief デストラクタ
  ~SimFFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 情報をセットする．
  void
  set(
    SizeType id,  ///< [in] ID番号
    SimNode* root ///< [in] 根のノード
  )
  {
    mId = id;
    mRoot = root;
  }

  /// @brief ID番号を得る．
  SizeType
  id() const
  {
    return mId;
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
  const std::vector<SimFault*>&
  fault_list() const
  {
    return mFaultList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 根のノード
  SimNode* mRoot{nullptr};

  // この FFR に属する故障のリスト
  std::vector<SimFault*> mFaultList;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMFFR_H
