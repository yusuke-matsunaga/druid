#ifndef FAULTGROUPGEN_H
#define FAULTGROUPGEN_H

/// @file FaultGroupGen.h
/// @brief FaultGroupGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TestCover.h"
#include "StructEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultGroupGen FaultGroupGen.h "FaultGroupGen.h"
/// @brief 両立故障グループを求めるクラス
///
/// 概念的には互いに両立な故障の極大グループ(極大両立集合)を求める
/// が，実際には故障検出用の拡張テストキューブ単位で両立集合を求める．
/// 結果としては一つの極大集合が一つの拡張テストキューブに対応する．
//////////////////////////////////////////////////////////////////////
class FaultGroupGen
{
public:

  /// @brief コンストラクタ
  FaultGroupGen(
    const TpgNetwork& network, ///< [in] 対象のネットワーク
    const JsonValue& option    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FaultGroupGen();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストベクタ集合を求める．
  vector<TestVector>
  generate(
    const vector<TestCover>& cover_list ///< [in] テストカバーのリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 拡張テストキューブを表すクラス
  struct ExCube {
    // ID番号
    SizeType mId;
    // 値の割り当てリスト
    AssignList mAssignments;
    // 対応する故障番号
    SizeType mFaultId;
    // ブロックリスト
    vector<SizeType> mBlockList;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障集合を初期化する．
  void
  init(
    const vector<TestCover>& cover_list
  );

  /// @brief 各キューブごとのブロックリストを作る．
  void
  gen_blocklist();

  /// @brief 拡張テストキューブに対する含意を行う．
  AssignList
  imply(
    const AssignList& assignments ///< [in] 元の割当
  );

  /// @brief 含意結果のチェックを行う．
  void
  check_imp(
    const AssignList& assignments0, ///< [in] 元の割当
    const AssignList& assignments1  ///< [in] 含意された割当
  );

  /// @brief 極大集合を求める．
  AssignList
  greedy_mcset();

  /// @brief 記録する．
  void
  record();

  /// @brief シグネチャから最も価値の低いインデックスを選ぶ．
  SizeType
  select_index(
    const vector<int>& signature
  );

  /// @brief 両立性のチェック
  bool
  is_compatible(
    const AssignList& assignments1, ///< [in] 割当1
    const AssignList& assignments2  ///< [in] 割当2
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 基本エンコーダ
  StructEngine mEngine;

  // 拡張テストキューブのリスト
  vector<const ExCube*> mCubeList;

  // 拡張テストキューブのブロックリスト
  vector<vector<SizeType>> mBlockListArray;

  // 拡張テストキューブの現在のブロックリスト
  vector<vector<SizeType>> mCurBlockListArray;

  // 残り故障数
  SizeType mFaultNum;

  // 各故障のカバー数
  // キーは故障番号
  vector<SizeType> mCountArray;

  // デバッグフラグ
  bool mDebug{false};

};

END_NAMESPACE_DRUID

#endif // FAULTGROUPGEN_H
