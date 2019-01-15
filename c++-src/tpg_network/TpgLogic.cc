
/// @file TpgLogic.cc
/// @brief TpgLogic の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogic.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
// クラス TpgLogic
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogic::TpgLogic(int id) :
  TpgNode(id)
{
}

// @brief デストラクタ
TpgLogic::~TpgLogic()
{
}

// @brief logic タイプの時 true を返す．
bool
TpgLogic::is_logic() const
{
  return true;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogic0
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogic0::TpgLogic0(int id) :
  TpgLogic(id)
{
}

// @brief デストラクタ
TpgLogic0::~TpgLogic0()
{
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgLogic0::fanin_list() const
{
  return Array<const TpgNode*>(nullptr, 0, 0);
}

// @brief ファンイン数を得る．
int
TpgLogic0::fanin_num() const
{
  return 0;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
const TpgNode*
TpgLogic0::fanin(int pos) const
{
  ASSERT_NOT_REACHED;

  return nullptr;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogic1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin ファンイン
TpgLogic1::TpgLogic1(int id,
		     TpgNode* fanin) :
  TpgLogic(id),
  mFanin(fanin)
{
}

// @brief デストラクタ
TpgLogic1::~TpgLogic1()
{
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgLogic1::fanin_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(&mFanin), 0, 1);
}

// @brief ファンイン数を得る．
int
TpgLogic1::fanin_num() const
{
  return 1;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
const TpgNode*
TpgLogic1::fanin(int pos) const
{
  ASSERT_COND( pos == 0 );

  return mFanin;
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogic2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
//
// fanin_list.size() == 2 であることを仮定している．
TpgLogic2::TpgLogic2(int id,
		     const vector<TpgNode*>& fanin_list) :
  TpgLogic(id)
{
  ASSERT_COND( fanin_list.size() == 2 );

  mFanins[0] = fanin_list[0];
  mFanins[1] = fanin_list[1];
}

// @brief デストラクタ
TpgLogic2::~TpgLogic2()
{
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgLogic2::fanin_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mFanins), 0, 2);
}

// @brief ファンイン数を得る．
int
TpgLogic2::fanin_num() const
{
  return 2;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
const TpgNode*
TpgLogic2::fanin(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < 2 );

  return mFanins[pos];
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogic3
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
//
// fanin_list.size() == 3 であることを仮定している．
TpgLogic3::TpgLogic3(int id,
		     const vector<TpgNode*>& fanin_list) :
  TpgLogic(id)
{
  ASSERT_COND( fanin_list.size() == 3 );

  mFanins[0] = fanin_list[0];
  mFanins[1] = fanin_list[1];
  mFanins[2] = fanin_list[2];
}

// @brief デストラクタ
TpgLogic3::~TpgLogic3()
{
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgLogic3::fanin_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mFanins), 0, 3);
}

// @brief ファンイン数を得る．
int
TpgLogic3::fanin_num() const
{
  return 3;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
const TpgNode*
TpgLogic3::fanin(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < 3 );

  return mFanins[pos];
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogic4
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
// @param[in] fanin_list ファンインのリスト
//
// fanin_list.size() == 4 であることを仮定している．
TpgLogic4::TpgLogic4(int id,
		     const vector<TpgNode*>& fanin_list) :
  TpgLogic(id)
{
  ASSERT_COND( fanin_list.size() == 4 );

  mFanins[0] = fanin_list[0];
  mFanins[1] = fanin_list[1];
  mFanins[2] = fanin_list[2];
  mFanins[3] = fanin_list[3];
}

// @brief デストラクタ
TpgLogic4::~TpgLogic4()
{
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgLogic4::fanin_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mFanins), 0, 4);
}

// @brief ファンイン数を得る．
int
TpgLogic4::fanin_num() const
{
  return 4;
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
const TpgNode*
TpgLogic4::fanin(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < 4 );

  return mFanins[pos];
}


//////////////////////////////////////////////////////////////////////
// クラス TpgLogicN
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] id ID番号
TpgLogicN::TpgLogicN(int id) :
  TpgLogic(id),
  mFaninNum(0),
  mFanins(nullptr)
{
  // mFaninNum と mFanins は後で設定する．
}

// @brief デストラクタ
TpgLogicN::~TpgLogicN()
{
}

// @brief ファンイン数を得る．
int
TpgLogicN::fanin_num() const
{
  return mFaninNum;
}

// @brief ファンインのリストを得る．
Array<const TpgNode*>
TpgLogicN::fanin_list() const
{
  return Array<const TpgNode*>(mFanins, 0, mFaninNum);
}

// @brief ファンインを得る．
// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
const TpgNode*
TpgLogicN::fanin(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < fanin_num() );

  return mFanins[pos];
}

// @brief ファンインを設定する．
// @param[in] inode_list ファンインのリスト
//
// と同時にファンイン用の配列も確保する．
// 多入力ゲートのみ意味を持つ仮想関数
void
TpgLogicN::set_fanin(const vector<TpgNode*>& inode_list,
		     Alloc& alloc)
{
  mFaninNum = inode_list.size();
  mFanins = alloc.get_array<const TpgNode*>(mFaninNum);
  for ( auto i: Range(mFaninNum) ) {
    mFanins[i] = inode_list[i];
  }
}

END_NAMESPACE_DRUID
