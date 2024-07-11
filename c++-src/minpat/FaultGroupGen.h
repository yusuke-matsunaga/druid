#ifndef FAULTGROUPGEN_H
#define FAULTGROUPGEN_H

/// @file FaultGroupGen.h
/// @brief FaultGroupGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "FaultInfo.h"
#include "BaseEnc.h"


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
  using ExCube = NodeTimeValList;

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

  /// @brief 両立故障グループを求める．
  vector<vector<FaultInfo>>
  generate(
    const vector<FaultInfo>& finfo_list, ///< [in] 故障情報のリスト
    SizeType limit                       ///< [in] 求める要素数の上限
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 拡張テストキューブを表すクラス
  struct ExCube {
    // 値の割り当てリスト
    NodeTimeValList mAssignments;
    // 対応する故障番号
    SizeType mFaultId;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障集合を初期化する．
  void
  init();

  /// @brief 極大集合を求める．
  void
  greedy_mcset(
    FaultGroup& signature,
    SizeType count
  );

  /// @brief 現在の故障集合を記録する．
  void
  record();

  /// @brief 最も価値の高いキューブを選ぶ．
  /// @return キューブ番号を返す．
  ///
  /// 追加できるキューブがない場合は mCubeList.size()
  /// を返す．
  SizeType
  select_cube(
    SizeType count
  );

  /// @brief 追加後の候補故障の数を数える．
  SizeType
  count_faults(
    const NodeTimeValList& assignments
  );

  /// @brief 最も価値の低いキューブを選んで取り除く
  /// @return 取り除いたキューブ番号を返す．
  SizeType
  remove_cube();

  /// @brief 両立性のチェック
  bool
  is_compatible(
    const NodeTimeValList& assignments1, ///< [in] 割当1
    const NodeTimeValList& assignments2  ///< [in] 割当2
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 基本エンコーダ
  BaseEnc mBaseEnc;

  // 拡張テストキューブのリスト
  vector<ExCube> mCubeList;

  // 現在選択されている故障番号の集合を表すビットマップ
  vector<bool> mFaultSet;

  // 現在選択されているキューブ番号の集合を表すビットマップ
  vector<bool> mCubeSet;

  // 現在選択されている故障集合用の値の割り当てリスト
  NodeTimeValList mAssignments;

  // タブーリスト
  // キーはキューブ番号
  vector<SizeType> mTabuList;

  // 禁止期間
  SizeType mTenure;

};

END_NAMESPACE_DRUID

#endif // FAULTGROUPGEN_H
