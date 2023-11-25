#ifndef TD_CONEBASE_H
#define TD_CONEBASE_H

/// @file td/ConeBase.h
/// @brief ConeBase のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "td/td_nsdef.h"
#include "td/StructEnc.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID_TD

//////////////////////////////////////////////////////////////////////
/// @class ConeBase ConeBase.h "td/ConeBase.h"
/// @brief FoCone, MffcCone に共通な基底クラス
//////////////////////////////////////////////////////////////////////
class ConeBase
{
protected:

  /// @brief コンストラクタ
  /// @param[in] struct_sat StructEnc ソルバ
  ConeBase(StructEnc& struct_sat);

  /// @brief デストラクタ
  ~ConeBase();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号の最大値を返す．
  ymuint
  max_id() const;

  /// @brief TFO ノード数を得る．
  ymuint
  tfo_num() const;

  /// @brief TFO ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < tfo_num() )
  const TpgNode*
  tfo_node(ymuint pos) const;

  /// @brief このコーンに関係する出力数を得る．
  ymuint
  output_num() const;

  /// @brief このコーンに関係する出力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  const TpgNode*
  output_node(ymuint pos) const;

  /// @brief このコーンに関係する出力のリストを得る．
  const vector<const TpgNode*>&
  output_list() const;

  /// @brief 1時刻前の正常回路の変数マップを得る．
  const VidMap&
  hvar_map() const;

  /// @brief 正常回路の変数マップを得る．
  const VidMap&
  gvar_map() const;

  /// @brief 故障回路の変数マップを得る．
  const VidMap&
  fvar_map() const;

  /// @brief 伝搬条件の変数マップを得る．
  const VidMap&
  dvar_map() const;

  /// @brief 1時刻前の正常値の変数を得る．
  SatVarId
  hvar(const TpgNode* node) const;

  /// @brief 正常値の変数を得る．
  SatVarId
  gvar(const TpgNode* node) const;

  /// @brief 故障値の変数を得る．
  SatVarId
  fvar(const TpgNode* node) const;

  /// @brief 伝搬値の変数を得る．
  SatVarId
  dvar(const TpgNode* node) const;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 指定されたノードの TFO の TFI に印をつける．
  /// @param[in] node_list 起点となるノードのリスト
  /// @param[in] use_dvar D-var を使う時 true にする．
  void
  mark_tfo_tfi(const vector<const TpgNode*>& node_list,
	       bool use_dvar = true);

  /// @brief 指定されたノードの TFO の TFI に印をつける．
  /// @param[in] node 起点となるノード
  /// @param[in] use_dvar D-var を使う時 true にする．
  void
  mark_tfo_tfi(const TpgNode* node,
	       bool use_dvar = true);

  /// @brief node に関する故障伝搬条件を作る．
  /// @param[in] node 対象のノード
  void
  make_dchain_cnf(const TpgNode* node);

  /// @brief ノードに故障値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] fvar 故障値の変数番号
  void
  set_fvar(const TpgNode* node,
	   SatVarId fvar);

  /// @brief ノードに伝搬値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] dvar 伝搬値の変数番号
  void
  set_dvar(const TpgNode* node,
	   SatVarId dvar);

  /// @brief tfo マークを読む．
  /// @param[in] node 対象のノード
  bool
  tfo_mark(const TpgNode* node) const;

  /// @brief tfo マークをつける．
  /// @param[in] node 対象のノード
  void
  set_tfo_mark(const TpgNode* node);

  /// @brief end-mark を読む．
  /// @param[in] node 対象のノード
  bool
  end_mark(const TpgNode* node) const;

  /// @brief tfo マークをつける．
  /// @param[in] node 対象のノード
  void
  set_end_mark(const TpgNode* node);

  /// @brief SAT ソルバを得る．
  SatSolver&
  solver();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // struct SAT ソルバ
  StructEnc& mStructEnc;

  // ノードのIDの最大値
  ymuint mMaxNodeId;

  // ノードごとのいくつかのフラグをまとめた配列
  vector<std::uint8_t> mMarkArray;

  // 故障の TFO のノードリスト
  vector<const TpgNode*> mNodeList;

  // TFO のノード数
  ymuint mTfoNum;

  // 現在の故障に関係ありそうな外部出力のリスト
  vector<const TpgNode*> mOutputList;

  // 故障値の変数マップ
  VidMap mFvarMap;

  // 故障伝搬値の変数マップ
  VidMap mDvarMap;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード番号の最大値を返す．
inline
ymuint
ConeBase::max_id() const
{
  return mMaxNodeId;
}

// @brief TFO ノード数を得る．
inline
ymuint
ConeBase::tfo_num() const
{
  return mTfoNum;
}

// @brief TFO ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < tfo_num() )
inline
const TpgNode*
ConeBase::tfo_node(ymuint pos) const
{
  ASSERT_COND( pos < tfo_num() );
  return mNodeList[pos];
}

// @brief このコーンに関係する出力数を得る．
inline
ymuint
ConeBase::output_num() const
{
  return mOutputList.size();
}

// @brief このコーンに関係する出力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
inline
const TpgNode*
ConeBase::output_node(ymuint pos) const
{
  ASSERT_COND( pos < output_num() );
  return mOutputList[pos];
}

// @brief 出力のノードのリストを返す．
inline
const vector<const TpgNode*>&
ConeBase::output_list() const
{
  return mOutputList;
}

// @brief 1時刻前の正常回路の変数マップを得る．
inline
const VidMap&
ConeBase::hvar_map() const
{
  return mStructEnc.hvar_map();
}

// @brief 正常回路の変数マップを得る．
inline
const VidMap&
ConeBase::gvar_map() const
{
  return mStructEnc.gvar_map();
}

// @brief 故障回路の変数マップを得る．
inline
const VidMap&
ConeBase::fvar_map() const
{
  return mFvarMap;
}

// @brief 伝搬条件の変数マップを得る．
inline
const VidMap&
ConeBase::dvar_map() const
{
  return mDvarMap;
}

// @brief 1時刻前の正常値の変数を得る．
inline
SatVarId
ConeBase::hvar(const TpgNode* node) const
{
  return mStructEnc.hvar(node);
}

// @brief 正常値の変数を得る．
inline
SatVarId
ConeBase::gvar(const TpgNode* node) const
{
  return mStructEnc.gvar(node);
}

// @brief 故障値の変数を得る．
inline
SatVarId
ConeBase::fvar(const TpgNode* node) const
{
  return mFvarMap(node);
}

// @brief 伝搬値の変数を得る．
inline
SatVarId
ConeBase::dvar(const TpgNode* node) const
{
  return mDvarMap(node);
}

// @brief ノードに故障値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] fvar 故障値の変数番号
inline
void
ConeBase::set_fvar(const TpgNode* node,
		   SatVarId fvar)
{
  mFvarMap.set_vid(node, fvar);
}

// @brief ノードに伝搬値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] dvar 伝搬値の変数番号
inline
void
ConeBase::set_dvar(const TpgNode* node,
		   SatVarId dvar)
{
  mDvarMap.set_vid(node, dvar);
}

// @brief tfo マークを読む．
inline
bool
ConeBase::tfo_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMarkArray[node->id()] >> 0) & 1U);
}

// @brief tfo マークをつける．
inline
void
ConeBase::set_tfo_mark(const TpgNode* node)
{
  mMarkArray[node->id()] |= 1U;
  mNodeList.push_back(node);
  if ( node->is_ppo() || end_mark(node) ) {
    mOutputList.push_back(node);
  }
}

// @brief end マークを読む．
// @param[in] node 対象のノード
inline
bool
ConeBase::end_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMarkArray[node->id()] >> 1) & 1U);
}

// @brief end マークをつける．
// @param[in] node 対象のノード
inline
void
ConeBase::set_end_mark(const TpgNode* node)
{
  mMarkArray[node->id()] |= 2U;
}

// @brief SAT ソルバを得る．
inline
SatSolver&
ConeBase::solver()
{
  return mStructEnc.solver();
}

END_NAMESPACE_DRUID_TD

#endif // TD_CONEBASE_H
