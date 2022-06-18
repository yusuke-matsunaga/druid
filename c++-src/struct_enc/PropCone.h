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
  ///
  /// ブロックノードより先のノードは含めない．
  /// 通常 block_node は nullptr か root_node の dominator
  /// となっているはず．
  PropCone(
    StructEnc& struct_enc,     ///< [in] StructEnc ソルバ
    const TpgNode* root_node,  ///< [in] FFRの根のノード
    const TpgNode* block_node, ///< [in] ブロックノード
    bool detect		       ///< [in] 故障を検出する時に true にするフラグ
  );

  /// @brief デストラクタ
  virtual
  ~PropCone();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号の最大値を返す．
  SizeType
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
  virtual
  vector<SatLiteral>
  make_condition(
    const TpgNode* root ///< [in] 起点となるノード
  ) = 0;

  /// @brief 故障検出に必要な割り当てを求める．
  virtual
  NodeValList
  extract_condition(
    const SatModel& model, ///< [in] SAT のモデル
    const TpgNode* root	   ///< [in] 起点のノード
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
  const TpgNode*
  tfo_node(
    int pos   ///<  [in] 位置番号 ( 0 <= pos < tfo_num() )
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
  const TpgNode*
  output_node(
    int pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
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
  void
  set_fvar(
    const TpgNode* node, ///< [in] ノード
    SatLiteral fvar	 ///< [in] 故障値の変数リテラル
  )
  {
    mFvarMap.set_vid(node, fvar);
  }

  /// @brief ノードに伝搬値用の変数番号を割り当てる．
  void
  set_dvar(
    const TpgNode* node, ///< [in] ノード
    SatLiteral dvar	 ///< [in] 伝搬値の変数リテラル
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
  void
  mark_tfo(
    const TpgNode* node  ///< [in] 起点となるノード
  );

  /// @brief node に関する故障伝搬条件を作る．
  void
  make_dchain_cnf(
    const TpgNode* node///< [in] 対象のノード
  );

  /// @brief tfo マークを読む．
  bool
  tfo_mark(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return static_cast<bool>((mMarkArray[node->id()] >> 0) & 1U);
  }

  /// @brief tfo マークをつける．
    ///
  /// 実はいろいろなことをやっている．
  /// - マークをつける．
  /// - mNodeList に追加する．
  /// - 外部出力の場合，mOutputList に追加して end-mark をつける．
  /// - end-mark が付いたノードの場合，mOutputList に追加する．
  void
  set_tfo_mark(
    const TpgNode* node ///< [in] 対象のノード
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
  bool
  end_mark(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return static_cast<bool>((mMarkArray[node->id()] >> 1) & 1U);
  }

  /// @brief end マークをつける．
  void
  set_end_mark(
    const TpgNode* node ///< [in] 対象のノード
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
  SizeType mMaxNodeId;

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

END_NAMESPACE_DRUID_STRUCTENC

#endif // PROPCONE_H
