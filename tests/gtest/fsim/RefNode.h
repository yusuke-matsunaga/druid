#ifndef REFNODE_H
#define REFNODE_H

/// @file RefNode.h
/// @brief RefNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Val3.h"


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
    PrimType gate_type,                ///< [in] ゲートタイプ
    const vector<RefNode*>& fanin_list ///< [in] ファンインリスト
  ) : mGateType{gate_type},
      mFaninList{fanin_list}
  {
  }

  /// @brief デストラクタ
  ~RefNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値をセットする．
  void
  set_val(
    Val3 val
  )
  {
    mGval = val;
    mFval = val;
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

  /// @brief 出力値を計算する．
  Val3
  calc_val() const
  {
    switch ( mGateType ) {
    case PrimType::None: // 入力
      // なにもしない．
      break;
    case PrimType::C0: // 定数0
      mVal = Val3::_0;
      break;
    case PrimType::C1: // 定数1
      mVal = Val3::_1;
      break;
    case PrimType::Buff: // バッファ
      mVal = mFaninList[0]->get_val();
      break;
    case PrimType::Not: // インバーター
      mVal = ~mFaninList[0]->get_val();
    case PrimType::And: // AND
      mVal = Val3::_1;
      for ( auto inode: mFaninList ) {
	mVal &= inode->get_val();
      }
      break;
    case PrimType::Nand: // NAND
      mVal = Val3::_1;
      for ( auto inode: mFaninList ) {
	mVal &= inode->get_val();
      }
      mVal = ~mVal;
      break;
    case PrimType::Or: // OR
      mVal = Val3::_0;
      for ( auto inode: mFaninList ) {
	mVal |= inode->get_val();
      }
      break;
    case PrimType::Nor: // NOR
      mVal = Val3::_0;
      for ( auto inode: mFaninList ) {
	mVal |= inode->get_val();
      }
      mVal = ~mVal;
      break;
    case PrimType::Xor: // XOR
      mVal = Val3::_0;
      for ( auto inode: mFaninList ) {
	mVal ^= inode->get_val();
      }
      break;
    case PrimType::Xnor: // XNOR
      mVal = Val3::_1;
      for ( auto inode: mFaninList ) {
	mVal ^= inode->get_val();
      }
      break;
    }
    return mVal;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ゲートタイプ
  PrimType mGateType;

  // ファンインのリスト
  vector<RefNode*> mFaninList;

  // 正常値
  Val3 mGval{Val3::_X};

  // 故障値
  Val3 mFval{Val3::_X};

};

END_NAMESPACE_DRUID

#endif // REFNODE_H
