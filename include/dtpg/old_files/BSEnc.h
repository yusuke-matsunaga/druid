#ifndef BSENC_H
#define BSENC_H

/// @file BSEnc.h
/// @brief BSEnc のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_DRUID

class ConeInfo;
class BSInfo;
class VidMap;

//////////////////////////////////////////////////////////////////////
/// @class BSEnc BSEnc.h "BSEnc.h"
/// @brief Broad-Side 方式用の条件を表す CNF を生成する．
//////////////////////////////////////////////////////////////////////
class BSEnc
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief CNF の生成を行う．
  ///
  /// root からいずれかの外部出力へ故障の影響が伝搬する
  /// 条件を表す CNF を作る．
  static
  void
  encode(
    SatSolver& solver,         ///< [in] SATソルバ
    const ConeInfo& cone_info, ///< [in] 故障コーンの構造情報
    const BSInfo& bs_info,     ///< [in] 1時刻前の構造情報
    const VidMap& gvar_map,    ///< [in] 正常値の変数マップ
    VidMap& hvar_map           ///< [out] 1時刻前の正常値の変数マップ
  );

};

END_NAMESPACE_DRUID

#endif // BSENC_H
