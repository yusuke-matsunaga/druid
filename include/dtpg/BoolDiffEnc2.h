#ifndef BOOLDIFFENC2_H
#define BOOLDIFFENC2_H

/// @file BoolDiffEnc2.h
/// @brief BoolDiffEnc2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "BaseEnc.h"


BEGIN_NAMESPACE_DRUID

class Extractor;

//////////////////////////////////////////////////////////////////////
/// @class BoolDiffEnc2 BoolDiffEnc2.h "BoolDiffEnc2.h"
/// @brief ブール微分を表すCNFを生成するクラス
///
/// BaseEnc に部品として組み込んで用いる SubEnc の継承クラス
///
/// 起点となるノードを複数指定する．
/// ただし，実際に反転させるのは同時に1つのみ．
/// 個々のノードの値の反転がいずれかの外部出力まで伝搬する
/// 条件を表す変数を生成する．
/// この変数は prop_var() で取得できる．
///
/// prop_var() が true になったときの十分条件は
/// extract_sufficient_condition() で取得できる．
//////////////////////////////////////////////////////////////////////
class BoolDiffEnc2 :
  public SubEnc
{
public:

  /// @brief コンストラクタ
  BoolDiffEnc2(
    BaseEnc& base_enc,                       ///< [in] 親の BaseEnc
    const vector<const TpgNode*>& root_list, ///< [in] 起点のノードのリスト
    const JsonValue& option = JsonValue{}    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~BoolDiffEnc2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

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

  /// @brief 故障の活性化条件を返す．
  vector<SatLiteral>
  cvar_assumptions(
    const TpgNode* node ///< [in] 起点のノード
  ) const;

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition(
    const TpgNode* node ///< [in] 起点のノード
  );


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

  /// @brief 正常値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return base_enc().gvar(node);
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

  // 起点のノードの配列
  vector<const TpgNode*> mRootArray;

  // 起点のノード番号をキーにして mRootArray 上のインデックスを求めるための辞書
  unordered_map<SizeType, SizeType> mRootMap;

  // mRoot の TFO
  vector<const TpgNode*> mTfoList;

  // mRoot から到達可能な外部出力のリスト
  vector<const TpgNode*> mOutputList;

  // 微分結果を表す変数
  SatLiteral mPropVar;

  // 故障挿入用の制御変数
  vector<SatLiteral> mCVarArray;

  // 故障値を表す変数マップ
  VidMap mFvarMap;

  // 伝搬条件を表す変数マップ
  VidMap mDvarMap;

  // 十分条件を取り出すオブジェクト
  std::unique_ptr<Extractor> mExtractor;

};

END_NAMESPACE_DRUID

#endif // BOOLDIFFENC2_H
