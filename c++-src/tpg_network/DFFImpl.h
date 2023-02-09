#ifndef DFFIMPL_H
#define DFFIMPL_H

/// @file DFFImpl.h
/// @brief DFFImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DFFImpl DFFImpl.h "DFFImpl.h"
/// @brief TpgDFF の実装クラス
//////////////////////////////////////////////////////////////////////
class DFFImpl
{
  friend class TpgNetworkImpl;

public:

  /// @brief コンストラクタ
  DFFImpl() = default;

  /// @brief デストラクタ
  ~DFFImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const { return mId; }

  /// @brief 入力端子のノードを返す．
  const TpgNode*
  input() const { return mInput; }

  /// @brief 出力端子のノードを返す．
  const TpgNode*
  output() const { return mOutput; }

  /// @brief クロック端子のノードを返す．
  const TpgNode*
  clock() const { return mClock; }

  /// @brief クリア端子のノードを返す．
  const TpgNode*
  clear() const { return mClear; }

  /// @brief プリセット端子のノードを返す．
  const TpgNode*
  preset() const { return mPreset; }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 入力端子
  TpgNode* mInput;

  // 出力端子
  TpgNode* mOutput;

  // クロック端子
  TpgNode* mClock;

  // クリア端子
  TpgNode* mClear;

  // プリセット端子
  TpgNode* mPreset;

};

END_NAMESPACE_DRUID

#endif // DFFIMPL_H
