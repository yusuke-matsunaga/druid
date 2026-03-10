#ifndef FFRDOMCHECKER_H
#define FFRDOMCHECKER_H

/// @file FFRDomChecker.h
/// @brief FFRDomChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "dtpg/BdEngine.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFRDomChecker FFRDomChecker.h "FFRDomChecker.h"
/// @brief FFR 内の故障の支配関係を調べるクラス
///
/// 機能的には fault_list から支配されている故障を見つける処理を行う
/// が，マルチスレッド実行のために故障番号をキーにして削除の印
/// を保持する del_mark を用いて結果を表す．
/// ffr 単位でマルチスレッド実行を行う場合，個々のスレッドが
/// アクセスする領域は互いに排他的なのでインターロックが必要ない．
//////////////////////////////////////////////////////////////////////
class FFRDomChecker
{
public:

  /// @brief コンストラクタ
  ///
  /// - fault_list は ffr 内の故障でなければならない．
  /// - del_mark[fault.id()] が true となっている故障はスキップする．
  FFRDomChecker(
    const TpgFFR& ffr,              ///< [in] 対象の FFR
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    std::vector<bool>& del_mark,    ///< [in] 削除マーク
    const JsonValue& option = {}    ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FFRDomChecker() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の支配関係を調べる．
  ///
  /// 除外された故障は mDelMark[fault.id()] = true となっている．
  void
  run();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief fault1 が fault2 を支配している時 true を返す．
  bool
  _check(
    const TpgFault& fault1, ///< [in] 故障1
    const TpgFault& fault2  ///< [in] 故障2
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障リスト
  TpgFaultList mFaultList;

  // 削除マーク
  std::vector<bool>& mDelMark;

  // エンジン
  BdEngine mEngine;

  // 故障番号をキーにしてその故障の非検出条件のリテラルを保持する辞書
  std::unordered_map<SizeType, SatLiteral> mLitDict;

};

END_NAMESPACE_DRUID

#endif // FFRDOMCHECKER_H
