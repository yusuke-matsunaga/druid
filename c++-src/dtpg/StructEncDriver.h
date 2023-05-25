#ifndef STRUCTENCDRIVER_H
#define STRUCTENCDRIVER_H

/// @file StructEncDriver.h
/// @brief StructEncDriver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgDriver.h"


BEGIN_NAMESPACE_DRUID

class StructEnc;

//////////////////////////////////////////////////////////////////////
/// @class StructEncDriver StructEncDriver.h "StructEncDriver.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class StructEncDriver :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  StructEncDriver(
    DtpgMgr& mgr,                  ///< [in] 親のマネージャ
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    bool has_prev_state,	   ///< [in] 1時刻前の回路を持つ時 true
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  ) : DtpgDriver{mgr, network, has_prev_state, just_type, init_param}
  {
  }

  /// @brief デストラクタ
  ~StructEncDriver() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行なう．
  void
  gen_pattern(
    StructEnc& enc,       ///< [in] 回路構造をエンコードしたもの
    const TpgFault& fault ///< [in] 対象の故障
  );

};


//////////////////////////////////////////////////////////////////////
/// @class StructEncDriver_FFR StructEncDriver_FFR.h "StructEncDriver_FFR.h"
/// @brief FFR モードの DtpgDriver_se
//////////////////////////////////////////////////////////////////////
class StructEncDriver_FFR :
  public StructEncDriver
{
public:

  /// @brief コンストラクタ
  StructEncDriver_FFR(
    DtpgMgr& mgr,                  ///< [in] 親のマネージャ
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    bool has_prev_state,	   ///< [in] 1時刻前の回路を持つ時 true
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  ) : StructEncDriver{mgr, network, has_prev_state, just_type, init_param}
  {
  }

  /// @brief デストラクタ
  ~StructEncDriver_FFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};


//////////////////////////////////////////////////////////////////////
/// @class StructEncDriver_MFFC StructEncDriver_MFFC.h "StructEncDriver_MFFC.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class StructEncDriver_MFFC :
  public StructEncDriver
{
public:

  /// @brief コンストラクタ
  StructEncDriver_MFFC(
    DtpgMgr& mgr,                  ///< [in] 親のマネージャ
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    bool has_prev_state,	   ///< [in] 1時刻前の回路を持つ時 true
    const string& just_type,       ///< [in] 正当化のタイプ
    const SatInitParam& init_param ///< [in] SATソルバの初期化パラメータ
  ) : StructEncDriver{mgr, network, has_prev_state, just_type, init_param}
  {
  }

  /// @brief デストラクタ
  ~StructEncDriver_MFFC() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief テスト生成を行う．
  void
  run() override;

};

END_NAMESPACE_DRUID

#endif // STRUCTENCDRIVER_H
