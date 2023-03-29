#ifndef VERIFIER_H
#define VERIFIER_H

/// @file Verifier.h
/// @brief Verifier のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "Fsim.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Verifier Verifier.h "Verifier.h"
/// @brief テストパタンの検証を行うクラス
//////////////////////////////////////////////////////////////////////
class Verifier
{
public:

  /// @brief コンストラクタ
  Verifier(
    const TpgNetwork& network, ///< [in] ネットワーク
    bool has_previous_state,   ///< [in] 1時刻前の値を持つ時 true にする．
    bool has_x                 ///< [in] 3値のシミュレーションを行う時 true にする．
  );

  /// @brief デストラクタ
  ~Verifier() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンが正しいかチェックする．
  bool
  check(
    const vector<TpgFault>& fault_list, ///< [in] 対象の故障のリスト
    const vector<TestVector>& tv_list   ///< [in] テストパタンのリスト
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障シミュレータ
  Fsim mFsim;

  // 検出済みの印
  vector<bool> mMarks;

};

END_NAMESPACE_DRUID

#endif // VERIFIER_H
