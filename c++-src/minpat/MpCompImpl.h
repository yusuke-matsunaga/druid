#ifndef MPCOMPIMPL_H
#define MPCOMPIMPL_H

/// @file MpCompImpl.h
/// @brief MpCompImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "types/TestVector.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class MpCompImpl MpCompImpl.h "MpCompImpl.h"
/// @brief MpCompl の実装クラス
///
/// このクラスはほぼインターフェイスの定義のみを行う純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class MpCompImpl
{
public:

  /// @brief 派生クラスのオブジェクトを生成するクラスメソッド
  static
  std::unique_ptr<MpCompImpl>
  new_obj(
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief デストラクタ
  virtual
  ~MpCompImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief パタン圧縮の本体
  virtual
  std::vector<TestVector>
  _run(
    const std::vector<TestVector>& tv_list, ///< [in] 初期パタンリスト
    const TpgFaultList& fault_list,         ///< [in] 対象の故障リスト
    const ConfigParam& option               ///< [in] オプション
  ) = 0;

};

END_NAMESPACE_DRUID

#endif // MPCOMPIMPL_H
