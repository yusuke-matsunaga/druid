#ifndef DTPGENGINEDRIVER_H
#define DTPGENGINEDRIVER_H

/// @file DtpgEngineDriver.h
/// @brief DtpgEngineDriver のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"


BEGIN_NAMESPACE_DRUID

class DtpgEngine;

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngineDriver DtpgEngineDriver.h "DtpgEngineDriver.h"
/// @brief FFR 単位で DTPG の基本的な処理を DtpgDriver の継承クラス
//////////////////////////////////////////////////////////////////////
class DtpgEngineDriver :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  DtpgEngineDriver(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const string& just_type,	   ///< [in] Justifier の種類を表す文字列
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  ) : DtpgDriver{network, just_type, init_param}
  {
  }

  /// @brief デストラクタ
  ~DtpgEngineDriver() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  void
  gen_pattern(
    DtpgEngine& engine,   ///< [in] 回路構造をエンコードしたもの
    const TpgFault& fault ///< [in] 対象の故障
  );

};


//////////////////////////////////////////////////////////////////////
/// @class DtpgEngineDriver_FFR DtpgEngineDriver_FFR.h "DtpgEngineDriver_FFR.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class DtpgEngineDriver_FFR :
  public DtpgEngineDriver
{
public:

  /// @brief コンストラクタ
  DtpgEngineDriver_FFR(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  ) : DtpgEngineDriver{network, just_type, init_param}
  {
  }

  /// @brief デストラクタ
  ~DtpgEngineDriver_FFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};


//////////////////////////////////////////////////////////////////////
/// @class DtpgEngineDriver_MFFC DtpgEngineDriver_MFFC.h "DtpgEngineDriver_MFFC.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class DtpgEngineDriver_MFFC :
  public DtpgEngineDriver
{
public:

  /// @brief コンストラクタ
  DtpgEngineDriver_MFFC(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  ) : DtpgEngineDriver{network, just_type, init_param}
  {
  }

  /// @brief デストラクタ
  ~DtpgEngineDriver_MFFC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};

END_NAMESPACE_DRUID

#endif // DTPGENGINEDRIVER_H
