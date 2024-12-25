#ifndef BOOLDIFFENC_H
#define BOOLDIFFENC_H

/// @file BoolDiffEnc.h
/// @brief BoolDiffEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

class Extractor;

//////////////////////////////////////////////////////////////////////
/// @class BoolDiffEnc BoolDiffEnc.h "BoolDiffEnc.h"
/// @brief ブール微分を表すCNFを生成するクラス
///
/// StructEngine に部品として組み込んで用いる SubEnc の継承クラス
///
/// 起点となるノードにおける値の反転がいずれかの外部出力に伝搬する
/// 条件を表す変数を生成する．
/// 生成された変数は prop_var() で取得できる．
///
/// prop_var() が true になったときの十分条件は
/// extract_sufficient_condition() で取得できる．
//////////////////////////////////////////////////////////////////////
class BoolDiffEnc :
  public SubEnc
{
public:

  /// @brief コンストラクタ
  BoolDiffEnc(
    StructEngine& engine,   ///< [in] 親の StructEngine
    const TpgNode* root,    ///< [in] 起点のノード
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~BoolDiffEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 起点のノードを返す．
  const TpgNode*
  root_node() const
  {
    return mRoot;
  }

  /// @brief root_node() の TFO を返す．
  const vector<const TpgNode*>&
  tfo_list() const
  {
    return mTfoList;
  }

  /// @brief root_node() から到達可能な外部出力のリストを返す．
  const vector<const TpgNode*>&
  output_list() const
  {
    return mOutputList;
  }

  /// @brief 微分結果を表す変数を返す．
  SatLiteral
  prop_var() const
  {
    return mPropVar;
  }

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition();

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignExpr
  extract_sufficient_conditions();

  /// @brief CNF 式を作成する際のサイズを見積もる．
  ///
  /// 実際には CNF 式は生成しない．
  CnfSize
  calc_cnf_size();


private:
  //////////////////////////////////////////////////////////////////////
  // SubEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 必要な変数を割り当て CNF 式を作る．
  void
  make_cnf() override;

  /// @brief 関連するノードのリストを返す．
  const vector<const TpgNode*>&
  node_list() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  void
  make_dchain_cnf(
    const TpgNode* node  ///< [in] 対象のノード
  );

  /// @brief 故障伝搬条件を表すCNF式のサイズを見積もる．
  CnfSize
  calc_dchain_cnf_size(
    const TpgNode* node  ///< [in] 対象のノード
  );

  /// @brief 正常値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return engine().gvar(node);
  }

  /// @brief 故障値を表す変数を返す．
  SatLiteral
  fvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mFvarMap(node);
  }

  /// @brief 故障伝搬条件を表す変数を返す．
  SatLiteral
  dvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return mDvarMap(node);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 起点のノード
  const TpgNode* mRoot;

  // mRoot の TFO
  vector<const TpgNode*> mTfoList;

  // mRoot から到達可能な外部出力のリスト
  vector<const TpgNode*> mOutputList;

  // 微分結果を表す変数
  SatLiteral mPropVar;

  // 故障値を表す変数マップ
  VidMap mFvarMap;

  // 伝搬条件を表す変数マップ
  VidMap mDvarMap;

  // 十分条件を取り出すオブジェクト
  std::unique_ptr<Extractor> mExtractor;

};

END_NAMESPACE_DRUID

#endif // BOOLDIFFENC_H
