#ifndef BASEENC_H
#define BASEENC_H

/// @file BaseEnc.h
/// @brief BaseEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "VidMap.h"
#include "Justifier.h"
#include "NodeTimeValList.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

class SubEnc;

//////////////////////////////////////////////////////////////////////
/// @class BaseEnc BaseEnc.h "BaseEnc.h"
/// @brief DTPG 用の基本的なエンコードを行うクラス
///
/// SATソルバとノード用の変数マップを持つ．
/// 具体的な仕事は SubEnc の継承クラスを登録することで行う．
///
/// make_cnf() などの関数
//////////////////////////////////////////////////////////////////////
class BaseEnc
{
  friend class SubEnc;

public:

  /// @brief コンストラクタ
  BaseEnc(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] 初期化オプション
    = JsonValue{}              ///<      "sat_param": JsonValue
                               ///<                   SATソルバの初期化パラメータ
                               ///<      "justifier": string
                               ///<                   Justifier の初期化パラメータ
  );

  /// @brief デストラクタ
  ///
  /// 保持している SubEnc はここで開放される．
  ~BaseEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 部品を登録する．
  ///
  /// subenc の所有権は BaseEnc に委譲される．
  void
  reg_subenc(
    SubEnc* subenc ///< [in] 部品のエンコーダ
  )
  {
    mSubEncList.push_back(subenc);
  }

  /// @brief 回路の構造を表すCNFを生成する．
  void
  make_cnf();

  /// @brief 与えられた割り当てを満足する外部入力の割り当てを求める．
  /// @return 外部入力の割り当てリストを返す．
  ///
  /// 事前にSAT問題の充足解が求められている必要がある．
  NodeTimeValList
  justify(
    const NodeTimeValList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief 値割り当てを対応するリテラルに変換する．
  SatLiteral
  conv_to_literal(
    NodeTimeVal assign ///< [in] 値割り当て
  );

  /// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
  vector<SatLiteral>
  conv_to_literal_list(
    NodeTimeValList& assign_list ///< [in] 値割り当てのリスト
  );

  /// @brief 対象のネットワークを得る．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief 関連するノードのリスト
  const vector<const TpgNode*>&
  cur_node_list() const
  {
    return mCurNodeList;
  }

  /// @brief SATソルバを返す．
  SatSolver&
  solver()
  {
    return mSolver;
  }

  /// @brief ノードの値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return mGvarMap(node);
  }

  /// @brief ノードの1時刻前の値を表す変数を返す．
  SatLiteral
  hvar(
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    return mHvarMap(node);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 部品のリスト
  vector<SubEnc*> mSubEncList;

  // 現時刻に関係のあるノードのリスト
  vector<const TpgNode*> mCurNodeList;

  // DFF の入力ノードのリスト
  vector<const TpgNode*> mDffInputList;

  // 1時刻前に関係のあるノードのリスト
  vector<const TpgNode*> mPrevNodeList;

  // SATソルバ
  SatSolver mSolver;

  // ノード用の変数マップ
  VidMap mGvarMap;

  // 1時刻前のノード用の変数マップ
  VidMap mHvarMap;

  // justifier
  Justifier mJustifier;

};


//////////////////////////////////////////////////////////////////////
/// @class SubEnc SubEnc.h "SubEnc.h"
/// @brief BaseEnc の部品クラス
///
/// このクラスは実際の継承クラスの純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class SubEnc
{
  friend class BaseEnc;

public:

  /// @brief コンストラクタ
  SubEnc(
    BaseEnc& base_enc ///< [in] 親の BaseEnc
  ) : mBaseEnc{base_enc}
  {
  }

  /// @brief デストラクタ
  virtual
  ~SubEnc() {}


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 親の BaseEnc を返す．
  BaseEnc&
  base_enc() const
  {
    return mBaseEnc;
  }

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return base_enc().network();
  }

  /// @brief SATソルバーを返す．
  SatSolver&
  solver()
  {
    return base_enc().solver();
  }

  /// @brief 値割り当てを対応するリテラルに変換する．
  SatLiteral
  conv_to_literal(
    NodeTimeVal assign ///< [in] 値割り当て
  )
  {
    return base_enc().conv_to_literal(assign);
  }

  /// @brief 値割り当てのリストを対応するリテラルのリストに変換する．
  vector<SatLiteral>
  conv_to_literal_list(
    NodeTimeValList& assign_list ///< [in] 値割り当てのリスト
  )
  {
    return base_enc().conv_to_literal_list(assign_list);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // BaseEnc から利用される仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を割り当てCNFを生成する．
  virtual
  void
  make_cnf() = 0;

  /// @brief 関連するノードのリストを返す．
  virtual
  const vector<const TpgNode*>&
  node_list() const = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 親の BaseEnc
  BaseEnc& mBaseEnc;

};

END_NAMESPACE_DRUID

#endif // BASEENC_H
