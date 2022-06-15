#ifndef PROPCONE_H
#define PROPCONE_H

/// @file PropCone.h
/// @brief PropCone のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "structenc_nsdef.h"
#include "StructEnc.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID_STRUCTENC

//////////////////////////////////////////////////////////////////////
/// @class PropCone PropCone.h "PropCone.h"
/// @brief 故障箇所の TFO に印をつけるためのクラス
//////////////////////////////////////////////////////////////////////
class PropCone
{
public:

  /// @brief コンストラクタ
  /// @param[in] struct_sat StructEnc ソルバ
  /// @param[in] root_node FFRの根のノード
  /// @param[in] block_node ブロックノード
  /// @param[in] detect 故障を検出する時に true にするフラグ
  ///
  /// ブロックノードより先のノードは含めない．
  /// 通常 block_node は nullptr か root_node の dominator
  /// となっているはず．
  PropCone(
    StructEnc& struct_sat,
    const TpgNode* root_node,
    const TpgNode* block_node,
    bool detect
  );

  /// @brief デストラクタ
  virtual
  ~PropCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号の最大値を返す．
  int
  max_id() const
  {
    return mMaxNodeId;
  }

  /// @brief 関係するノードの変数を作る．
  virtual
  void
  make_vars() = 0;

  /// @brief 関係するノードの入出力の関係を表すCNFを作る．
  virtual
  void
  make_cnf() = 0;

  /// @brief 故障の影響伝搬させる条件を作る．
  /// @param[in] root 起点となるノード
  /// @param[out] assumptions 結果の仮定を表すリテラルのリスト
  virtual
  void
  make_prop_condition(
    const TpgNode* root,
    vector<SatLiteral>& assumptions
  ) = 0;

  /// @brief 故障検出に必要な割り当てを求める．
  /// @param[in] model SAT のモデル
  /// @param[in] root 起点のノード
  /// @param[out] 値の割り当て結果を入れるリスト
  virtual
  NodeValList
  extract(
    const SatModel& model,
    const TpgNode* root
  );

  /// @brief 根のノードを得る．
  const TpgNode*
  root_node() const
  {
    return mNodeList[0];
  }

  /// @brief TFO ノード数を得る．
  int
  tfo_num() const
  {
    return mNodeList.size();
  }

  /// @brief TFO ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < tfo_num() )
  const TpgNode*
  tfo_node(
    int pos
  ) const
  {
    ASSERT_COND( pos < tfo_num() );
    return mNodeList[pos];
  }

  /// @brief TFO ノードのリストを得る．
  const vector<const TpgNode*>&
  tfo_node_list() const
  {
    return mNodeList;
  }

  /// @brief このコーンに関係する出力数を得る．
  int
  output_num() const
  {
    return mOutputList.size();
  }

  /// @brief このコーンに関係する出力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  const TpgNode*
  output_node(
    int pos
  ) const
  {
    ASSERT_COND( pos < output_num() );
    return mOutputList[pos];
  }

  /// @brief このコーンに関係する出力のリストを得る．
  const vector<const TpgNode*>&
  output_list() const
  {
    return mOutputList;
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正常回路の変数マップを得る．
  const VidMap&
  gvar_map() const
  {
    return mStructEnc.var_map(1);
  }

  /// @brief 故障回路の変数マップを得る．
  const VidMap&
  fvar_map() const
  {
    return mFvarMap;
  }

  /// @brief 伝搬条件の変数マップを得る．
  const VidMap&
  dvar_map() const
  {
    return mDvarMap;
  }

  /// @brief 正常値の変数を得る．
  SatLiteral
  gvar(
    const TpgNode* node
  ) const
  {
    return mStructEnc.var(node, 1);
  }

  /// @brief 故障値の変数を得る．
  SatLiteral
  fvar(
    const TpgNode* node
  ) const
  {
    return mFvarMap(node);
  }

  /// @brief 伝搬値の変数を得る．
  SatLiteral
  dvar(
    const TpgNode* node
  ) const
  {
    return mDvarMap(node);
  }

  /// @brief ノードに故障値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] fvar 故障値の変数リテラル
  void
  set_fvar(
    const TpgNode* node,
    SatLiteral fvar
  )
  {
    mFvarMap.set_vid(node, fvar);
  }

  /// @brief ノードに伝搬値用の変数番号を割り当てる．
  /// @param[in] node ノード
  /// @param[in] dvar 伝搬値の変数リテラル
  void
  set_dvar(
    const TpgNode* node,
    SatLiteral dvar
  )
  {
    mDvarMap.set_vid(node, dvar);
  }

  /// @brief StructEnc を得る．
  StructEnc&
  struct_sat()
  {
    return mStructEnc;
  }

  /// @brief SAT ソルバを得る．
  SatSolver&
  solver()
  {
    return mStructEnc.solver();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 指定されたノードの TFO に印をつける．
  /// @param[in] node 起点となるノード
  void
  mark_tfo(
    const TpgNode* node
  );

  /// @brief node に関する故障伝搬条件を作る．
  /// @param[in] node 対象のノード
  void
  make_dchain_cnf(
    const TpgNode* node
  );

  /// @brief tfo マークを読む．
  /// @param[in] node 対象のノード
  bool
  tfo_mark(
    const TpgNode* node
  ) const
  {
    return static_cast<bool>((mMarkArray[node->id()] >> 0) & 1U);
  }

  /// @brief tfo マークをつける．
  /// @param[in] node 対象のノード
  ///
  /// 実はいろいろなことをやっている．
  /// - マークをつける．
  /// - mNodeList に追加する．
  /// - 外部出力の場合，mOutputList に追加して end-mark をつける．
  /// - end-mark が付いたノードの場合，mOutputList に追加する．
  void
  set_tfo_mark(
    const TpgNode* node
  )
  {
    int id = node->id();
    if ( ((mMarkArray[id] >> 0) & 1U) == 0U ) {
      mMarkArray[id] |= 1U;
      mNodeList.push_back(node);
      if ( node->is_ppo() ) {
	set_end_mark(node);
	mOutputList.push_back(node);
      }
      else if ( end_mark(node) ) {
	mOutputList.push_back(node);
      }
    }
  }

  /// @brief end-mark を読む．
  /// @param[in] node 対象のノード
  bool
  end_mark(
    const TpgNode* node
  ) const
  {
    return static_cast<bool>((mMarkArray[node->id()] >> 1) & 1U);
  }

  /// @brief end マークをつける．
  /// @param[in] node 対象のノード
  void
  set_end_mark(
    const TpgNode* node
  )
  {
    mMarkArray[node->id()] |= 2U;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // struct SAT ソルバ
  StructEnc& mStructEnc;

  // 故障検出フラグ
  bool mDetect;

  // ノードのIDの最大値
  int mMaxNodeId;

  // ノードごとのいくつかのフラグをまとめた配列
  vector<ymuint8> mMarkArray;

  // 故障の TFO のノードリスト
  vector<const TpgNode*> mNodeList;

  // 現在の故障に関係ありそうな外部出力のリスト
  vector<const TpgNode*> mOutputList;

  // 故障値の変数マップ
  VidMap mFvarMap;

  // 故障伝搬値の変数マップ
  VidMap mDvarMap;

};


#if 0
//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード番号の最大値を返す．
inline
int
PropCone::max_id() const
{
  return mMaxNodeId;
}

// @brief 根のノードを得る．
inline
const TpgNode*
PropCone::root_node() const
{
  return mNodeList[0];
}

// @brief TFO ノード数を得る．
inline
int
PropCone::tfo_num() const
{
  return mNodeList.size();
}

// @brief TFO ノードを得る．
// @param[in] pos 位置番号 ( 0 <= pos < tfo_num() )
inline
const TpgNode*
PropCone::tfo_node(int pos) const
{
  ASSERT_COND( pos < tfo_num() );
  return mNodeList[pos];
}

// @brief TFO ノードのリストを得る．
inline
const vector<const TpgNode*>&
PropCone::tfo_node_list() const
{
  return mNodeList;
}

// @brief このコーンに関係する出力数を得る．
inline
int
PropCone::output_num() const
{
  return mOutputList.size();
}

// @brief このコーンに関係する出力を得る．
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
inline
const TpgNode*
PropCone::output_node(int pos) const
{
  ASSERT_COND( pos < output_num() );
  return mOutputList[pos];
}

// @brief 出力のノードのリストを返す．
inline
const vector<const TpgNode*>&
PropCone::output_list() const
{
  return mOutputList;
}

// @brief 正常回路の変数マップを得る．
inline
const VidMap&
PropCone::gvar_map() const
{
  return mStructEnc.var_map(1);
}

// @brief 故障回路の変数マップを得る．
inline
const VidMap&
PropCone::fvar_map() const
{
  return mFvarMap;
}

// @brief 伝搬条件の変数マップを得る．
inline
const VidMap&
PropCone::dvar_map() const
{
  return mDvarMap;
}

// @brief 正常値の変数を得る．
inline
SatLiteral
PropCone::gvar(const TpgNode* node) const
{
  return mStructEnc.var(node, 1);
}

// @brief 故障値の変数を得る．
inline
SatLiteral
PropCone::fvar(const TpgNode* node) const
{
  return mFvarMap(node);
}

// @brief 伝搬値の変数を得る．
inline
SatLiteral
PropCone::dvar(const TpgNode* node) const
{
  return mDvarMap(node);
}

// @brief ノードに故障値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] fvar 故障値の変数番号
inline
void
PropCone::set_fvar(const TpgNode* node,
		   SatLiteral fvar)
{
  mFvarMap.set_vid(node, fvar);
}

// @brief ノードに伝搬値用の変数番号を割り当てる．
// @param[in] node ノード
// @param[in] dvar 伝搬値の変数番号
inline
void
PropCone::set_dvar(const TpgNode* node,
		   SatLiteral dvar)
{
  mDvarMap.set_vid(node, dvar);
}

// @brief tfo マークを読む．
inline
bool
PropCone::tfo_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMarkArray[node->id()] >> 0) & 1U);
}

// @brief tfo マークをつける．
inline
void
PropCone::set_tfo_mark(const TpgNode* node)
{
  int id = node->id();
  if ( ((mMarkArray[id] >> 0) & 1U) == 0U ) {
    mMarkArray[id] |= 1U;
    mNodeList.push_back(node);
    if ( node->is_ppo() ) {
      set_end_mark(node);
      mOutputList.push_back(node);
    }
    else if ( end_mark(node) ) {
      mOutputList.push_back(node);
    }
  }
}

// @brief end マークを読む．
// @param[in] node 対象のノード
inline
bool
PropCone::end_mark(const TpgNode* node) const
{
  return static_cast<bool>((mMarkArray[node->id()] >> 1) & 1U);
}

// @brief end マークをつける．
// @param[in] node 対象のノード
inline
void
PropCone::set_end_mark(const TpgNode* node)
{
  mMarkArray[node->id()] |= 2U;
}

// @brief StructEnc を得る．
inline
StructEnc&
PropCone::struct_sat()
{
  return mStructEnc;
}

// @brief SAT ソルバを得る．
inline
SatSolver&
PropCone::solver()
{
  return mStructEnc.solver();
}
#endif

END_NAMESPACE_DRUID_STRUCTENC

#endif // PROPCONE_H
