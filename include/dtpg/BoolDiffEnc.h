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
/// @ingroup DtpgGroup
/// @sa StructEngine
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
    const TpgNode& root,                  ///< [in] 起点のノード
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief コンストラクタ
  BoolDiffEnc(
    const TpgNode& root,                  ///< [in] 起点のノード
    const TpgNodeList& output_list,       ///< [in] 出力のリスト
    const JsonValue& option = JsonValue{} ///< [in] オプション
  );

  /// @brief デストラクタ
  ~BoolDiffEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 起点のノードを返す．
  TpgNode
  root_node() const
  {
    return mRoot;
  }

  /// @brief root_node() の TFO を返す．
  const TpgNodeList&
  tfo_list() const
  {
    return mTfoList;
  }

  /// @brief root_node() から到達可能な外部出力のリストを返す．
  const TpgNodeList&
  output_list() const
  {
    return mOutputList;
  }

  /// @brief root_node() から到達可能な外部出力の数を返す．
  SizeType
  output_num() const
  {
    return mOutputList.size();
  }

  /// @brief root_node() から到達可能な外部出力を返す．
  TpgNode
  output(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const
  {
    if ( pos >= output_num() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mOutputList[pos];
  }

  /// @brief 微分結果を表す変数を返す．
  SatLiteral
  prop_var() const
  {
    engine().update();
    return mPropVar;
  }

  /// @brief 微分結果を表す変数を返す．
  SatLiteral
  prop_var(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  ) const
  {
    if ( pos >= output_num() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    engine().update();
    return mPropVarList[pos];
  }

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition();

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition(
    SizeType pos ///< [in] 出力番号 ( 0 <= pos < output_num() )
  );


private:
  //////////////////////////////////////////////////////////////////////
  // SubEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief データ構造の初期化を行う．
  void
  init() override;

  /// @brief 必要な変数を割り当て CNF 式を作る．
  void
  make_cnf() override;

  /// @brief 関連するノードのリストを返す．
  const TpgNodeList&
  node_list() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  void
  make_dchain_cnf(
    const TpgNode& node  ///< [in] 対象のノード
  );

  /// @brief 正常値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode& node ///< [in] 対象のノード
  )
  {
    return engine().gvar(node);
  }

  /// @brief 故障値を表す変数を返す．
  SatLiteral
  fvar(
    const TpgNode& node ///< [in] 対象のノード
  )
  {
    return mFvarMap(node);
  }

  /// @brief 故障伝搬条件を表す変数を返す．
  SatLiteral
  dvar(
    const TpgNode& node ///< [in] 対象のノード
  )
  {
    return mDvarMap(node);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 起点のノード
  TpgNode mRoot;

  // mRoot の TFO
  TpgNodeList mTfoList;

  // mRoot から到達可能な外部出力のリスト
  TpgNodeList mOutputList;

  // 微分結果を表す変数
  SatLiteral mPropVar;

  // 各出力ごとの微分結果を表す変数
  std::vector<SatLiteral> mPropVarList;

  // 故障値を表す変数マップ
  VidMap mFvarMap;

  // 伝搬条件を表す変数マップ
  VidMap mDvarMap;

  // 十分条件を取り出すオブジェクト
  std::unique_ptr<Extractor> mExtractor;

};

END_NAMESPACE_DRUID

#endif // BOOLDIFFENC_H
