#ifndef DTPG2IMPL_H
#define DTPG2IMPL_H

/// @file Dtpg2Impl.h
/// @brief Dtpg2Impl のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DtpgImpl.h"

#include "ym/RandGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Dtpg2Impl Dtpg2Impl.h "Dtpg2Impl.h"
/// @brief Dtpg2 の実装用のクラス
//////////////////////////////////////////////////////////////////////
class Dtpg2Impl :
  public DtpgImpl
{
public:

  /// @brief コンストラクタ
  /// @param[in] sat_type SATソルバの種類を表す文字列
  /// @param[in] sat_option SATソルバに渡すオプション文字列
  /// @param[in] sat_outp SATソルバ用の出力ストリーム
  /// @param[in] bt バックトレーサー
  /// @param[in] network 対象のネットワーク
  /// @param[in] root 故障伝搬の起点となるノード
  Dtpg2Impl(const string& sat_type,
	    const string& sat_option,
	    ostream* sat_outp,
	    BackTracer& bt,
	    const TpgNetwork& network,
	    const TpgNode* root);

  /// @brief デストラクタ
  ~Dtpg2Impl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief XOR制約のための変数リストを作る．
  /// @return 変数の数を返す．
  int
  make_xor_list();

  /// @brief XOR制約を追加する．
  void
  add_xor_constraint(int num,
		     RandGen& randgen);

  /// @brief テスト生成を行なう．
  /// @param[in] fault 対象の故障
  /// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
  /// @param[inout] stats DTPGの統計情報
  /// @return 結果を返す．
  SatBool3
  dtpg_with_xor(const TpgFault* fault,
		int xor_assign,
		NodeValList& nodeval_list,
		DtpgStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// XOR制約を作る．
  SatLiteral
  make_xor(const vector<SatVarId>& var_list,
	   int start,
	   int end);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  int mXorNum1;
  int mXorNum2;
  vector<const TpgNode*> mXorNodeList;

  vector<SatLiteral> mXorLitList;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_DRUID

#endif // DTPG2IMPL_H
