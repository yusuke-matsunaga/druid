#ifndef CONEENC_H
#define CONEENC_H

/// @file ConeEnc.h
/// @brief ConeEnc のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

class ConeInfo;
class VidMap;

//////////////////////////////////////////////////////////////////////
/// @class ConeEnc ConeEnc.h "ConeEnc.h"
/// @brief DTPG 用の基本的なエンコーディングを行うクラス
///
/// 実は static メソッドのみのクラス
//////////////////////////////////////////////////////////////////////
class ConeEnc
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件を表す CNF の生成を行う．
  static
  void
  encode(
    SatSolver& solver,    ///< [in] SAT ソルバ
    const ConeInfo& info, ///< [in] 故障コーンの情報
    VidMap& gvar_map,     ///< [out] 正常値用の変数マップ
    VidMap& fvar_map      ///< [out] 故障値用の変数マップ
  );

};

END_NAMESPACE_DRUID

#endif // CONEENC_H
