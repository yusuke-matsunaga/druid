#ifndef REFNODE_H
#define REFNODE_H

/// @file RefNode.h
/// @brief RefNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/Val3.h"
#include "ym/PrimType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class RefNode RefNode.h "RefNode.h"
/// @brief RefSim で用いるノード
//////////////////////////////////////////////////////////////////////
class RefNode
{
public:

  /// @brief コンストラクタ
  RefNode(
    SizeType id,                       ///< [in] ノード番号
    PrimType gate_type,                ///< [in] ゲートタイプ
    const std::vector<RefNode*>& fanin_list ///< [in] ファンインリスト
  ) : mId{id},
      mGateType{gate_type},
      mFaninList{fanin_list}
  {
  }

  /// @brief デストラクタ
  ~RefNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を得る．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 正常値をセットする．
  void
  set_gval(
    Val3 val
  )
  {
    mGval = val;
  }

  /// @brief 故障値をセットする．
  void
  set_fval(
    Val3 val
  )
  {
    mFval = val;
  }

  /// @brief 正常値をシフトする．
  void
  shift_gval()
  {
    mHval = mGval;
  }

  /// @brief 正常値を得る．
  Val3
  get_gval() const
  {
    return mGval;
  }

  /// @brief 故障値を得る．
  Val3
  get_fval() const
  {
    return mFval;
  }

  /// @brief 1時刻前の正常値を得る．
  Val3
  get_hval() const
  {
    return mHval;
  }

  /// @brief 正常値を計算する．
  Val3
  calc_gval()
  {
    switch ( mGateType ) {
    case PrimType::None: // 入力
      // なにもしない．
      break;
    case PrimType::C0: // 定数0
      mGval = Val3::_0;
      break;
    case PrimType::C1: // 定数1
      mGval = Val3::_1;
      break;
    case PrimType::Buff: // バッファ
      mGval = mFaninList[0]->get_gval();
      break;
    case PrimType::Not: // インバーター
      mGval = ~mFaninList[0]->get_gval();
      break;
    case PrimType::And: // AND
      mGval = Val3::_1;
      for ( auto inode: mFaninList ) {
	mGval = mGval & inode->get_gval();
      }
      break;
    case PrimType::Nand: // NAND
      mGval = Val3::_1;
      for ( auto inode: mFaninList ) {
	mGval = mGval & inode->get_gval();
      }
      mGval = ~mGval;
      break;
    case PrimType::Or: // OR
      mGval = Val3::_0;
      for ( auto inode: mFaninList ) {
	mGval = mGval | inode->get_gval();
      }
      break;
    case PrimType::Nor: // NOR
      mGval = Val3::_0;
      for ( auto inode: mFaninList ) {
	mGval = mGval | inode->get_gval();
      }
      mGval = ~mGval;
      break;
    case PrimType::Xor: // XOR
      mGval = Val3::_0;
      for ( auto inode: mFaninList ) {
	mGval = mGval ^ inode->get_gval();
      }
      break;
    case PrimType::Xnor: // XNOR
      mGval = Val3::_1;
      for ( auto inode: mFaninList ) {
	mGval = mGval ^ inode->get_gval();
      }
      break;
    }
    return mGval;
  }

  /// @brief 故障値を計算する．
  Val3
  calc_fval()
  {
    switch ( mGateType ) {
    case PrimType::None: // 入力
      // なにもしない．
      break;
    case PrimType::C0: // 定数0
      mFval = Val3::_0;
      break;
    case PrimType::C1: // 定数1
      mFval = Val3::_1;
      break;
    case PrimType::Buff: // バッファ
      mFval = mFaninList[0]->get_fval();
      break;
    case PrimType::Not: // インバーター
      mFval = ~mFaninList[0]->get_fval();
      break;
    case PrimType::And: // AND
      mFval = Val3::_1;
      for ( auto inode: mFaninList ) {
	mFval = mFval & inode->get_fval();
      }
      break;
    case PrimType::Nand: // NAND
      mFval = Val3::_1;
      for ( auto inode: mFaninList ) {
	mFval = mFval & inode->get_fval();
      }
      mFval = ~mFval;
      break;
    case PrimType::Or: // OR
      mFval = Val3::_0;
      for ( auto inode: mFaninList ) {
	mFval = mFval | inode->get_fval();
      }
      break;
    case PrimType::Nor: // NOR
      mFval = Val3::_0;
      for ( auto inode: mFaninList ) {
	mFval = mFval | inode->get_fval();
      }
      mFval = ~mFval;
      break;
    case PrimType::Xor: // XOR
      mFval = Val3::_0;
      for ( auto inode: mFaninList ) {
	mFval = mFval ^ inode->get_fval();
      }
      break;
    case PrimType::Xnor: // XNOR
      mFval = Val3::_1;
      for ( auto inode: mFaninList ) {
	mFval = mFval ^ inode->get_fval();
      }
      break;
    }
    return mFval;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // ゲートタイプ
  PrimType mGateType;

  // ファンインのリスト
  std::vector<RefNode*> mFaninList;

  // 正常値
  Val3 mGval{Val3::_X};

  // 故障値
  Val3 mFval{Val3::_X};

  // 1時刻前の値
  Val3 mHval{Val3::_X};

};

END_NAMESPACE_DRUID

#endif // REFNODE_H
