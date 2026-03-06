#ifndef MPCOMP_H
#define MPCOMP_H

/// @file MpComp.h
/// @brief MpComp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpComp MpComp.h "MpComp.h"
/// @brief パタン圧縮を行うクラス
///
/// このクラスはほぼインターフェイスの定義のみを行う純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class MpComp
{
public:

  /// @brief 派生クラスのオブジェクトを生成するクラスメソッド
  static
  std::unique_ptr<MpComp>
  new_obj(
    const std::string& type  ///< [in] 種類を表す文字列
  );

  /// @brief デストラクタ
  virtual
  ~MpComp() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テストパタンの圧縮を行う．
  std::vector<TestVector>
  run(
    const std::vector<TestVector>& tv_list, ///< [in] 初期パタンリスト
    const TpgFaultList& fault_list,         ///< [in] 対象の故障リスト
    const JsonValue& option = {}            ///< [in] オプション
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief パタン圧縮の本体
  virtual
  std::vector<TestVector>
  _run(
    const std::vector<TestVector>& tv_list, ///< [in] 初期パタンリスト
    const TpgFaultList& fault_list,         ///< [in] 対象の故障リスト
    const JsonValue& option                 ///< [in] オプション
  ) = 0;

};

END_NAMESPACE_DRUID

#endif // MPCOMP_H
