#ifndef JUSTIFIER_H
#define JUSTIFIER_H

/// @file Justifier.h
/// @brief Justifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

class JustImpl;

//////////////////////////////////////////////////////////////////////
/// @class Justifier Justifier.h "td/Justifier.h"
/// @brief 正当化に必要な割当を求めるファンクター
//////////////////////////////////////////////////////////////////////
class Justifier
{
public:

  /// @brief コンストラクタ
  /// @param[in] type 種類を表す文字列
  /// @param[in] network 対象のネットワーク
  Justifier(const string& type,
	    const TpgNetwork& network);

  /// @brief デストラクタ
  ~Justifier();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 正当化に必要な割当を求める(縮退故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var_map 変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @return テストベクタ
  TestVector
  operator()(const NodeValList& assign_list,
	     const VidMap& var_map,
	     const vector<SatBool3>& model);

  /// @brief 正当化に必要な割当を求める(遷移故障用)．
  /// @param[in] assign_list 値の割り当てリスト
  /// @param[in] var1_map 1時刻目の変数番号のマップ
  /// @param[in] var2_map 2時刻目の変数番号のマップ
  /// @param[in] model SAT問題の解
  /// @return テストベクタ
  TestVector
  operator()(const NodeValList& assign_list,
	     const VidMap& var1_map,
	     const VidMap& var2_map,
	     const vector<SatBool3>& model);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  // input_num, dff_num を取得するために用いる．
  const TpgNetwork& mNetwork;

  // 実装クラス
  std::unique_ptr<JustImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // JUSTIFIER_H
