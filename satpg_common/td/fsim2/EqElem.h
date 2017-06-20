﻿#ifndef EQELEM_H
#define EQELEM_H

/// @file EqElem.h
/// @brief EqElem のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.

#include "fsim2_nsdef.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM2

class SimNode;
class EventQ;

//////////////////////////////////////////////////////////////////////
/// @class EqElem EqElem.h "EqElem.h"
/// @brief EventQ の要素
//////////////////////////////////////////////////////////////////////
class EqElem
{
  friend class EventQ;

protected:

  /// @brief コンストラクタ
  EqElem();

  /// @brief デストラクタ
  virtual
  ~EqElem();


private:
  //////////////////////////////////////////////////////////////////////
  // EventQ のみがアクセスする関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューに積まれていたら true を返す．
  bool
  in_queue() const;

  /// @brief キューフラグをセットする．
  void
  set_queue();

  /// @brief キューフラグをクリアする．
  void
  clear_queue();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // キューにつまれているかどうかを示すフラグ
  bool mInQueue;

  // イベントキューの次の要素
  SimNode* mLink;

};


//////////////////////////////////////////////////////////////////////
// EqElem のインライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
EqElem::EqElem() :
  mInQueue(false),
  mLink(nullptr)
{
}

// @brief デストラクタ
inline
EqElem::~EqElem()
{
}

// @brief キューに積まれていたら true を返す．
inline
bool
EqElem::in_queue() const
{
  return mInQueue;
}

// @brief キューフラグをセットする．
inline
void
EqElem::set_queue()
{
  mInQueue = true;
}

// @brief キューフラグをクリアする．
inline
void
EqElem::clear_queue()
{
  mInQueue = false;
}

END_NAMESPACE_YM_SATPG_FSIM2

#endif // EQELEM_H
