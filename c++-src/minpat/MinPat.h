#ifndef MINPAT_H
#define MINPAT_H

/// @file MinPat.h
/// @brief MinPat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgNetwork.h"
#include "types/TpgFaultList.h"
#include "types/AssignList.h"
#include "dtpg/StructEngine.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MinPat MinPat.h "MinPat.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class MinPat
{
public:

  /// @brief コンストラクタ
  MinPat(
    const TpgNetwork& network
  );

  /// @brief デストラクタ
  ~MinPat() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタン圧縮を行う．
  std::vector<TestVector>
  run(
    const TpgFaultList& fault_list,
    const JsonValue& option = {}
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  std::vector<AssignList>
  mincov(
    const std::vector<AssignList>& pat_list,
    const std::vector<AssignList>& aux_list
  );

  std::vector<AssignList>
  packing(
    const std::vector<AssignList>& pat_list
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  TpgNetwork mNetwork;

  // 検出可能な故障リスト
  TpgFaultList mDetFaultList;

  // 故障番号のリスト
  std::vector<SizeType> mFidList;

  // 故障番号からローカルな位置番号を各方した辞書
  // mFidList の逆関数になっている．
  std::unordered_map<SizeType, SizeType> mFidMap;

  // 構造推論エンジン
  StructEngine mEngine;

};

END_NAMESPACE_DRUID

#endif // MINPAT_H
