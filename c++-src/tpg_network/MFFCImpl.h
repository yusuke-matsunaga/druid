#ifndef MFFCIMPL_H
#define MFFCIMPL_H

/// @file MFFCImpl.h
/// @brief MFFCImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MFFCImpl MFFCImpl.h "MFFCImpl.h"
/// @brief TpgMFFC の実装クラス
//////////////////////////////////////////////////////////////////////
class MFFCImpl
{
  friend class TpgNetworkImpl;

public:

  /// @brief コンストラクタ
  MFFCImpl() = default;

  /// @brief デストラクタ
  ~MFFCImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @breif ID番号を返す．
  SizeType
  id() const { return mId; }

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const { return mRoot; }

  /// @brief FFRの配列を返す．
  const vector<TpgFFR>&
  ffr_list() const { return mFFRList; }

  /// @brief 故障の配列
  const vector<const TpgFault*>&
  fault_list() const { return mFaultList; }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 根のノード
  const TpgNode* mRoot{nullptr};

  // FFRの配列
  vector<TpgFFR> mFFRList;

  // 故障の配列
  vector<const TpgFault*> mFaultList;

};

END_NAMESPACE_DRUID

#endif // MFFCIMPL_H
