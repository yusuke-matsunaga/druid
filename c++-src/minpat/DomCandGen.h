#ifndef DOMCANDGEN_H
#define DOMCANDGEN_H

/// @file DomCandGen.h
/// @brief DomCandGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"
#include "PackedVal.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FaultInfo;

//////////////////////////////////////////////////////////////////////
/// @class DomCandGen DomCandGen.h "DomCandGen.h"
/// @brief 故障シミュレーションを用いて支配故障の候補を作るクラス
///
/// 故障シミュレーションの結果，f1 が検出されたパタンの全てで f2 が
/// 検出されている場合に，f2 が f1 に支配されている候補となる．
/// 処理の流れとしてはネットワークと故障リスト，およびテストパタンを
/// 与えて結果を受け取ればよいのでただの関数の形で実装することも可能
/// だが，内部にデータ構造を構築したほうが効率良いのでクラスとしている．
//////////////////////////////////////////////////////////////////////
class DomCandGen
{
public:

  /// @brief コンストラクタ
  DomCandGen(
    const TpgNetwork& network,                 ///< [in] 対象のネットワーク
    const vector<const TpgFault*>& fault_list, ///< [in] 対象の故障のリスト
    const vector<TestVector>& tv_list          ///< [in] テストベクタのリスト
  );

  /// @brief デストラクタ
  ~DomCandGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 支配故障の候補リストを作る．
  ///
  /// 結果は故障番号をキーにして dom_cand_list に格納する．
  void
  run(
    SizeType loop_limit,                           ///< [in] 変化がなくなった後に回す
                                                   ///<      のループ回数の上限
    vector<vector<const TpgFault*>>& dom_cand_list ///< [out] 支配する故障候補のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 一回の故障シミュレーションを行う．
  /// @retval true 支配故障のリストに変化があった．
  /// @retval false 変化がなかった．
  bool
  do_fsim(
    const vector<TestVector>& tv_list,             ///< [in] テストベクタのリスト
    vector<vector<const TpgFault*>>& dom_cand_list ///< [out] 支配する故障候補のリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 作業領域
  struct Work {

    // 故障シミュレーションの検出パタン
    PackedVal mPat{PV_ALL0};

    // mDomCandList が初期化されているかを示すフラグ
    bool mHasDomCandList{false};

  };

  // 故障のリスト
  const vector<const TpgFault*>& mFaultList;

  // 故障番号の最大値 + 1
  SizeType mMaxFaultId;

  // テストベクタのリスト
  const vector<TestVector>& mTvList;

  // 故障シミュレータ
  Fsim mFsim;

  // 1時刻前の状態を保持する時 true にするフラグ
  bool mHasPrevState;

  // 入力数
  SizeType mInputNum;

  // DFF数
  SizeType mDffNum;

  // 作業領域の配列
  // サイズは mMaxFaultId
  // キーは故障番号
  vector<Work> mWorkArray;

};

END_NAMESPACE_DRUID

#endif // DOMCANDGEN_H
