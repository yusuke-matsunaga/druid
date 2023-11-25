#ifndef TD_FOCONE_H
#define TD_FOCONE_H

/// @file td/FoCone.h
/// @brief FoCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/ConeBase.h"


BEGIN_NAMESPACE_DRUID_TD

//////////////////////////////////////////////////////////////////////
/// @class FoCone FoCone.h "td/FoCone.h"
/// @brief 故障箇所の TFO に印をつけるためのクラス
//////////////////////////////////////////////////////////////////////
class FoCone :
  public ConeBase
{
  friend class StructEnc;

private:

  /// @brief コンストラクタ
  /// @param[in] struct_sat StructEnc ソルバ
  /// @param[in] fnode 故障位置のノード
  /// @param[in] bnode ブロックノード
  /// @param[in] detect 検出条件
  ///
  /// ブロックノードより先のノードは含めない．
  /// 通常 bnode は fnode の dominator
  /// となっているはず．
  FoCone(StructEnc& struct_sat,
	 const TpgNode* fnode,
	 const TpgNode* bnode,
	 Val3 detect);

  /// @brief デストラクタ
  ~FoCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 十分条件を得る．
  /// @param[in] sat_model SAT の割り当て結果
  /// @param[in] fault 故障
  /// @param[out] suf_list 十分条件の割り当てリスト
  void
  get_suf_list(const vector<SatBool3>& sat_model,
	       const TpgFault* fault,
	       NodeValList& suf_list) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_DRUID_TD

#endif // TD_FOCONE_H
