#ifndef BOOLDIFFENC2_H
#define BOOLDIFFENC2_H

/// @file BoolDiffEnc2.h
/// @brief BoolDiffEnc2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "StructEngine.h"


BEGIN_NAMESPACE_DRUID

class Extractor;

//////////////////////////////////////////////////////////////////////
/// @class BoolDiffEnc2 BoolDiffEnc2.h "BoolDiffEnc2.h"
/// @brief ブール微分を表すCNFを生成するクラス
///
/// StructEngine に部品として組み込んで用いる SubEnc の継承クラス
///
/// 起点となるノードにおける値を0/1に固定した影響が個々の外部出力
/// に伝搬する条件を表す変数を生成する．
/// 生成された変数は prop_var() で取得できる．
///
/// prop_var() が true になったときの十分条件は
/// extract_sufficient_condition() で取得できる．
///
/// BoolDiffEnc との違いは個々の外部出力ごとの伝搬可能性を調べていること．
//////////////////////////////////////////////////////////////////////
class BoolDiffEnc2 :
  public SubEnc
{
public:

  /// @brief コンストラクタ
  BoolDiffEnc2(
    StructEngine& engine,   ///< [in] 親の StructEngine
    const TpgNode* root,    ///< [in] 起点のノード
    const JsonValue& option ///< [in] オプション
    = JsonValue{}
  );

  /// @brief デストラクタ
  ~BoolDiffEnc2();


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

  /// @brief root_node() から到達可能な外部出力のリストを返す．
  const std::vector<const TpgNode*>&
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
  const TpgNode*
  output(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const
  {
    if ( pos >= output_num() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mOutputList[pos];
  }

  /// @brief root_node() の TFO を返す．
  const std::vector<const TpgNode*>&
  tfo_list(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const
  {
    return mPartialTfoList[pos];
  }

  /// @brief 微分結果を表す変数を返す．
  SatLiteral
  prop_var(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const
  {
    if ( pos >= output_num() ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mPropVarList[pos];
  }

  /// @brief 直前の check() が成功したときの十分条件を求める．
  AssignList
  extract_sufficient_condition(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  );


private:
  //////////////////////////////////////////////////////////////////////
  // SubEnc の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 必要な変数を割り当て CNF 式を作る．
  void
  make_cnf() override;

  /// @brief 関連するノードのリストを返す．
  const std::vector<const TpgNode*>&
  node_list() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬条件を表すCNF式を生成する．
  void
  make_dchain_cnf(
    const TpgNode* node,  ///< [in] 対象のノード
    SizeType pos          ///< [in] 出力番号
  );

  /// @brief 正常値を表す変数を返す．
  SatLiteral
  gvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    return engine().gvar(node);
  }

  /// @brief 0故障値を表す変数を返す．
  SatLiteral
  fvar(
    const TpgNode* node ///< [in] 対象のノード
  )
  {
    if ( pos >= output_num() ) {
      throw std::out_of_range{"pos is out of range"};
    }
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
  std::vector<const TpgNode*> mTfoList;

  // mRoot から到達可能な外部出力のリスト
  std::vector<const TpgNode*> mOutputList;

  // 個々の出力ごとの TFO
  std::vector<std::vector<const TpgNode*>> mPartialTfoList;

  // 微分結果を表す変数のリスト
  std::vector<SatLiteral> mPropVarList;

  // 故障値を表す変数マップのリスト
  std::vector<VidMap> mFvarMapList;

  // 伝搬条件を表す変数マップのリスト
  std::vector<VidMap> mDvarMapList;

  // 十分条件を取り出すオブジェクト
  std::unique_ptr<Extractor> mExtractor;

};

END_NAMESPACE_DRUID

#endif // BOOLDIFFENC2_H
