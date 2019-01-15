#ifndef TD_MFFCCONE_H
#define TD_MFFCCONE_H

/// @file td/MffcCone.h
/// @brief MffcCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/ConeBase.h"


BEGIN_NAMESPACE_DRUID_TD

//////////////////////////////////////////////////////////////////////
/// @class MffcCone MffcCone.h "td/MffcCone.h"
/// @brief MFFC 内の FFR の根に故障を挿入するためのクラス
//////////////////////////////////////////////////////////////////////
class MffcCone :
  public ConeBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] struct_sat StructEnc ソルバ
  /// @param[in] fnode MFFC の根のノード
  MffcCone(StructEnc& struct_sat,
	   const TpgNode* fnode);

  /// @brief デストラクタ
  ~MffcCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief MFFC の根のノードを返す．
  const TpgNode*
  mffc_root() const;

  /// @brief 故障挿入箇所数を得る．
  ///
  /// 具体的には mffc_root()->mffc_elem_num() + 1
  /// を返す．
  ymuint
  mffc_elem_num() const;

  /// @brief 故障挿入箇所を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
  const TpgNode*
  mffc_elem(ymuint pos) const;

  /// @brief 故障挿入用の変数を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
  SatVarId
  mffc_elem_var(ymuint pos) const;

  /// @brief 故障挿入位置を選ぶ．
  /// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
  /// @param[out] assumptions 結果の割り当てを追加するベクタ
  void
  select_fault_node(ymuint pos,
		    vector<SatLiteral>& assumptions) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障挿入位置のリスト
  vector<const TpgNode*> mElemList;

  // 故障挿入用の変数のリスト
  vector<SatVarId> mElemVarList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief MFFC の根のノードを返す．
inline
const TpgNode*
MffcCone::mffc_root() const
{
  return mElemList[0];
}

// @brief 故障挿入箇所数を得る．
//
// 具体的には mffc_root()->mffc_elem_num() + 1
// を返す．
inline
ymuint
MffcCone::mffc_elem_num() const
{
  return mElemList.size();
}

// @brief 故障挿入箇所を得る．
// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
inline
const TpgNode*
MffcCone::mffc_elem(ymuint pos) const
{
  ASSERT_COND( pos < mffc_elem_num() );
  return mElemList[pos];
}

// @brief 故障挿入用の変数を得る．
// @param[in] pos 位置番号 ( 0 <= pos < mffc_elem_num() )
inline
SatVarId
MffcCone::mffc_elem_var(ymuint pos) const
{
  ASSERT_COND( pos < mffc_elem_num() );
  return mElemVarList[pos];
}

END_NAMESPACE_DRUID_TD

#endif // TD_MFFCCONE_H
