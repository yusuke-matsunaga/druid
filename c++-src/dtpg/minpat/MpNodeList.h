#ifndef MPNODELIST_H
#define MPNODELIST_H

/// @file MpNodeList.h
/// @brief MpNodeList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class MpColGraph;

//////////////////////////////////////////////////////////////////////
/// @class MpNodeList MpNodeList.h "MpNodeList.h"
/// @brief MpColGraph の隣接ノードリストを表すクラス
//////////////////////////////////////////////////////////////////////
class MpNodeList
{
  public;

  using iterator = MpNodeIterator;

public:

  /// @brief コンストラクタ
  MpNodeList(const MpColGraph& graph,
	     int num,
	     int* body);

  /// @brief デストラクタ
  ~MpNodeList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 先頭の反復子を返す．
  MpNodeIterator
  begin() const;

  /// @brief 末尾の反復子を返す．
  MpNodeIterator
  end() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // グラフ
  const MpColGraph& mGraph;

  // 要素数
  int mNum;

  // 要素の配列
  int* mBody;

};


//////////////////////////////////////////////////////////////////////
/// @class MpNodeIterator MpNodeList.h "MpNodeList.h"
/// @brief MpNodeList の反復子
//////////////////////////////////////////////////////////////////////
class MpNodeIterator
{
public:

  /// @brief コンストラクタ
  /// @param[in] graph 対象のグラフ
  ///
  MpNodeIterator(const MpColGraph& graph,
		 int* ptr1);

  /// @brief デストラクタ
  ~MpNodeIterator();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief dereference 演算子
  int
  operator*() const;

  /// @brief increment 演算子
  const MpNodeIterator&
  operator++();

  /// @brief 等価比較演算子
  bool
  operator==(const MpNodeIterator& right) const;

  /// @brief 非等価比較演算子
  bool
  operator!=(const MpNodeIterator& right) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のグラフ
  const MpColGraph& mGraph;

  // 第１レベルのポインタ
  int* mPtr1;

  // 第２レベルのポインタ
  int* mPtr2;

  // 第２レベルポインタの末尾
  int* mEnd2;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
MpNodeList::MpNodeList(const MpColGraph& graph,
		       int num,
		       int* body) :
  mGraph(graph),
  mNum(num),
  mBody(body)
{
}

// @brief デストラクタ
inline
MpNodeList::~MpNodeList()
{
}

// @brief 先頭の反復子を返す．
inline
MpNodeIterator
MpNodeList::begin() const
{
  return MpNodeIterator(mGraph, mBody);
}

// @brief 末尾の反復子を返す．
inline
MpNodeIterator
MpNodeList::end() const
{
  return mMpNodeIterator(mGraph, mBody + mNum);
}

// @brief コンストラクタ
// @param[in] graph 対象のグラフ
//
inline
MpNodeIterator::MpNodeIterator(const MpColGraph& graph,
			       int* ptr1) :
  mGraph(graph),
  mPtr1(ptr1),
  mPtr2(nullptr),
  mEnd2(nullptr)
{
}

// @brief デストラクタ
inline
MpNodeIterator::~MpNodeIterator()
{
}

// @brief dereference 演算子
inline
int
MpNodeIterator::operator*() const
{
  if ( mPtr2 == nullptr ) {
    int id = *mPtr1;
    const IdList& id_list = mGraph.mConflictPairArray[id];
    mPtr2 = id_list.mBody;
    mEnd2 = id_list.mBody + id_list.mNum;
  }
  return *mPtr2;
}

// @brief increment 演算子
inline
const MpNodeIterator&
MpNodeIterator::operator++()
{
  ++ mPtr2;
  if ( mPtr2 == mEnd2 ) {
    ++ mPtr1;
    mPtr2 = nullptr;
  }
}

// @brief 等価比較演算子
inline
bool
MpNodeIterator::operator==(const MpNodeIterator& right) const
{
  if ( &mGraph != &right.mGraph ) {
    return false;
  }
  if ( mPtr1 != right.mPtr1 ) {
    return false;
  }
  if ( mPtr2 != right.mPtr2 ) {
    return false;
  }
  return true;
}

// @brief 非等価比較演算子
inline
bool
MpNodeIterator::operator!=(const MpNodeIterator& right) const
{
  return !operator==(right);
}

END_NAMESPACE_DRUID

#endif // MPNODELIST_H
