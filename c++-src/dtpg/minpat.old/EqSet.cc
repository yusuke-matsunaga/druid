
/// @file EqSet.cc
/// @brief EqSet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2015 Yusuke Matsunaga
/// All rights reserved.


#include "EqSet.h"
#include "TpgFault.h"
#include "Fsim.h"


BEGIN_NAMESPACE_DRUID_SA

//////////////////////////////////////////////////////////////////////
// クラス EqSet
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
EqSet::EqSet()
{
}

// @brief デストラクタ
EqSet::~EqSet()
{
}

// @brief 初期化を行う．
// @param[in] elem_list 要素番号のリスト
void
EqSet::init(const vector<ymuint>& elem_list)
{
  mElemArray.clear();
  mMarkArray.clear();

  mMaxId = 0;
  for (ymuint i = 0; i < elem_list.size(); ++ i) {
    ymuint id = elem_list[i];
    if ( mMaxId < id ) {
      mMaxId = id;
    }
  }
  ++ mMaxId;

  mElemArray.resize(mMaxId);
  mMarkArray.resize(mMaxId, false);

  mTop = nullptr;
  Elem* prev = nullptr;
  for (ymuint i = 0; i < elem_list.size(); ++ i) {
    ymuint id = elem_list[i];
    Elem* elem = &mElemArray[id];
    if ( mTop == nullptr ) {
      mTop = elem;
    }
    else {
      prev->mLink = elem;
    }
    elem->mId = id;
    elem->mLink = nullptr;
    elem->mNextTop = nullptr;
    prev = elem;
  }

  mNeedFinalize = true;
}

// @brief 細分化を行う．
// @param[in] elem_list 一方の部分集合の要素のリスト
// @return 変化があったら true を返す．
bool
EqSet::refinement(const vector<ymuint>& elem_list)
{
  // elem_list に含まれる要素に印をつける．
  for (ymuint i = 0; i < elem_list.size(); ++ i) {
    ymuint id = elem_list[i];
    mMarkArray[id] = 1UL;
  }

  bool chg = false;

  // 個々の同値類候補リストを mMarkArray の内容にしたがって分類する．
  Elem* next = nullptr;
  for (Elem* top = mTop; top != nullptr; top = next) {
    next = top->mNextTop;
    Elem* top0 = nullptr;
    Elem* prev0 = nullptr;
    Elem* top1 = nullptr;
    Elem* prev1 = nullptr;
    for (Elem* tmp = top; tmp != nullptr; tmp = tmp->mLink) {
      if ( mMarkArray[tmp->mId] ) {
	if ( top1 != nullptr ) {
	  prev1->mLink = tmp;
	}
	else {
	  top1 = tmp;
	}
	prev1 = tmp;
      }
      else {
	if ( top0 != nullptr ) {
	  prev0->mLink = tmp;
	}
	else {
	  top0 = tmp;
	}
	prev0 = tmp;
      }
    }
    if ( prev0 != nullptr ) {
      prev0->mLink = nullptr;
    }
    if ( prev1 != nullptr ) {
      prev1->mLink = nullptr;
    }
    if ( top == top0 ) {
      if ( top1 != nullptr ) {
	top0->mNextTop = top1;
	top1->mNextTop = next;
	mNeedFinalize = true;
	chg = true;
      }
    }
    else {
      if ( top0 != nullptr ) {
	top1->mNextTop = top0;
	top0->mNextTop = next;
	mNeedFinalize = true;
	chg = true;
      }
    }
  }

  // mMarkArray の印を消す．
  for (ymuint i = 0; i < elem_list.size(); ++ i) {
    ymuint id = elem_list[i];
    mMarkArray[id] = 0UL;
  }

  return chg;
}

// @brief 細分化を行う．
// @param[in] fsim 故障シミュレータ
// @return 変化があったら true を返す．
bool
EqSet::multi_refinement(Fsim& fsim)
{
  // シミュレーション結果 から mMarkArray を作る．
  ymuint n = fsim.det_fault_num();
  for (ymuint i = 0; i < n; ++ i) {
    const TpgFault* f = fsim.det_fault(i);
    PackedVal bv = fsim.det_fault_pat(i);
    mMarkArray[f->id()] = bv;
  }

  bool chg = false;
  Elem* next = nullptr;
  for (Elem* top = mTop; top != nullptr; top = next) {
    next = top->mNextTop;
    top->mNextTop = nullptr;
    Elem* cur_top = nullptr;
    Elem* cur_last = nullptr;
    Elem* tmp_link = nullptr;
    for (Elem* tmp = top; tmp != nullptr; tmp = tmp_link) {
      tmp_link = tmp->mLink;
      tmp->mLink = nullptr;
      PackedVal bv = mMarkArray[tmp->mId];
      Elem* rep = nullptr;
      for (Elem* tmp2 = cur_top; tmp2 != nullptr; tmp2 = tmp2->mNextTop) {
	if ( mMarkArray[tmp2->mId] == bv ) {
	  rep = tmp2;
	  break;
	}
      }
      if ( rep == nullptr ) {
	if ( cur_last == nullptr ) {
	  cur_top = tmp;
	}
	else {
	  cur_last->mNextTop = tmp;
	}
	cur_last = tmp;
      }
      else {
	Elem* tmp2 = rep;
	for ( ; tmp2->mLink != nullptr; tmp2 = tmp2->mLink) { }
	tmp2->mLink = tmp;
      }
    }
    ASSERT_COND( cur_top == top );
    ASSERT_COND( cur_last->mNextTop == nullptr );
    cur_last->mNextTop = next;
    if ( cur_top != cur_last ) {
      mNeedFinalize = true;
      chg = true;
    }
  }

  // mMarkArray の印を消す．
  for (ymuint i = 0; i < n; ++ i) {
    const TpgFault* f = fsim.det_fault(i);
    mMarkArray[f->id()] = 0UL;
  }

  return chg;
}

// @brief 同値類候補数を返す．
ymuint
EqSet::class_num() const
{
  finalize();

  return mTopArray.size();
}

// @brief 同値類候補を変える．
// @param[in] pos 位置番号 (0 <= pos < class_num())
// @param[out] elem_list 同値類の要素番号を収めるリスト
void
EqSet::class_list(ymuint pos,
		  vector<ymuint>& elem_list) const
{
  finalize();

  ASSERT_COND( pos < class_num() );
  Elem* top = mTopArray[pos];

  ymuint n = 0;
  for (Elem* tmp = top; tmp != nullptr; tmp = tmp->mLink, ++ n) { }
  elem_list.clear();
  elem_list.reserve(n);

  for (Elem* tmp = top; tmp != nullptr; tmp = tmp->mLink) {
    elem_list.push_back(tmp->mId);
  }
}

// @brief 最終的な処理を行う．
void
EqSet::finalize() const
{
  if ( !mNeedFinalize ) {
    return;
  }

  ymuint n = 0;
  for (Elem* top = mTop; top != nullptr; top = top->mNextTop, ++ n) {
    ;
  }

  mTopArray.clear();
  mTopArray.reserve(n);
  for (Elem* top = mTop; top != nullptr; top = top->mNextTop, ++ n) {
    mTopArray.push_back(top);
  }

  mNeedFinalize = false;
}

// @brief 内容を出力する．
void
EqSet::dump(ostream& s) const
{
  ymuint i = 0;
  for (Elem* top = mTop; top != nullptr; top = top->mNextTop, ++ i) {
    s << "[" << setw(4) << i << "]: ";
    for (Elem* tmp = top; tmp != nullptr; tmp = tmp->mLink) {
      s << " " << tmp->mId;
    }
    s << endl;
  }
  s << endl;
}

END_NAMESPACE_DRUID_SA
