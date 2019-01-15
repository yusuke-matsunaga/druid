#ifndef TPGFAULTBASE_H
#define TPGFAULTBASE_H

/// @file TpgFaultBase.h
/// @brief TpgFaultBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFault.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultBase TpgFaultBase.h "TpgFaultBase.h"
/// @brief TpgFault の実装クラス
///
/// set_rep() を TpgFault のメンバから外すためにこういう構造にしている．
//////////////////////////////////////////////////////////////////////
class TpgFaultBase :
  public TpgFault
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] name 故障位置のノード名
  /// @param[in] val 故障値
  /// @param[in] node 故障位置のノード
  /// @param[in] rep_fault 代表故障
  TpgFaultBase(int id,
	       int val,
	       const TpgNode* node,
	       const char* name,
	       TpgFault* rep_fault);

  /// @brief デストラクタ
  ~TpgFaultBase();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  int
  id() const override;

  /// @brief 故障値を返す．
  /// @note 返す値は 0 か 1
  int
  val() const override;

  /// @brief 代表故障を返す．
  ///
  /// 代表故障の時は自分自身を返す．
  const TpgFault*
  rep_fault() const override;


public:
  //////////////////////////////////////////////////////////////////////
  // 微妙な関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 代表故障を設定する．
  /// @param[in] rep 代表故障
  void
  set_rep(const TpgFault* rep);


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードを返す．
  const TpgNode*
  tpg_node() const;

  /// @brief ノード名を返す．
  const char*
  node_name() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号 + 故障値(最下位ビット)
  ymuint mIdVal;

  // 対象の TpgNode
  const TpgNode* mTpgNode;

  // ノード名
  const char* mNodeName;

  // 代表故障
  const TpgFault* mRepFault;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノードを返す．
inline
const TpgNode*
TpgFaultBase::tpg_node() const
{
  return mTpgNode;
}

// @brief ノード名を返す．
inline
const char*
TpgFaultBase::node_name() const
{
  return mNodeName;
}

END_NAMESPACE_DRUID

#endif // TPGFAULTBASE_H
