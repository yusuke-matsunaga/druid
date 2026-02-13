#ifndef DTPGDRIVER_H
#define DTPGDRIVER_H

/// @file DtpgDriver.h
/// @brief DtpgDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/DtpgMgr.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_DRUID

class DtpgStats;
class DtpgDriverImpl;

//////////////////////////////////////////////////////////////////////
/// @class DtpgDriver DtpgDriver.h "DtpgDriver.h"
/// @brief テスト生成を行う基底クラス
///
/// 実際の処理は DtpgDriverImpl が行う．
//////////////////////////////////////////////////////////////////////
class DtpgDriver
{
public:

  /// @brief ノード単位をテスト生成を行うオブジェクトを生成する．
  static
  DtpgDriver
  node_driver(
    const TpgNode& node,    ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option ///< [in] オプション
  );

  /// @brief FFR単位でテスト生成を行うオブジェクトを生成する．
  static
  DtpgDriver
  ffr_driver(
    const TpgFFR& ffr,	    ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option ///< [in] オプション
  );

  /// @brief MFFC単位でテスト生成を行うオブジェクトを生成する．
  static
  DtpgDriver
  mffc_driver(
    const TpgMFFC& mffc,    ///< [in] 故障伝搬の起点となる MFFC
    const JsonValue& option ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgDriver();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のテストパタンを求める．
  void
  gen_pattern(
    const TpgFault& fault, ///< [in] 対象の故障
    DtpgResults& results,  ///< [out] テスト生成の結果
    DtpgStats& stats       ///< [out] 統計情報
  );

  /// @brief CNF の生成時間を返す．
  double
  cnf_time() const;

  /// @brief SATの統計情報を返す．
  SatStats
  sat_stats() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  DtpgDriver(
    DtpgDriverImpl* impl ///< [in] 実装クラス
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装クラスのポインタ
  std::unique_ptr<DtpgDriverImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // DTPGDRIVER_H
