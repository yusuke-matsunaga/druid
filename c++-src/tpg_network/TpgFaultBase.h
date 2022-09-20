#ifndef TPGFAULTBASE_H
#define TPGFAULTBASE_H

/// @file TpgFaultBase.h
/// @brief TpgFaultBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
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
  TpgFaultBase(
    SizeType id,         ///< [in] ID番号
    Fval2 val,		 ///< [in] 故障位置のノード名
    const TpgNode* node, ///< [in] 故障値
    const string& name,	 ///< [in] 故障位置のノード
    TpgFault* rep_fault	 ///< [in] 代表故障
  );

  /// @brief デストラクタ
  ~TpgFaultBase();


public:
  //////////////////////////////////////////////////////////////////////
  // read-only のメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const override;

  /// @brief 故障値を返す．
  Fval2
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
  void
  set_rep(
    const TpgFault* rep  ///< [in] 代表故障
  );


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードを返す．
  const TpgNode*
  tpg_node() const
  {
    return mTpgNode;
  }

  /// @brief ノード名を返す．
  const string&
  node_name() const
  {
    return mNodeName;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号 + 故障値(最下位ビット)
  ymuint mIdVal;

  // 対象の TpgNode
  const TpgNode* mTpgNode;

  // ノード名
  const string mNodeName;

  // 代表故障
  const TpgFault* mRepFault;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTBASE_H
