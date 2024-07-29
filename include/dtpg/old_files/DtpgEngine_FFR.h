#ifndef DTPGENGINE_FFR_H
#define DTPGENGINE_FFR_H

/// @file DtpgEngine_FFR.h
/// @brief DtpgEngine_FFR のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine_FFR DtpgEngine_FFR.h "DtpgEngine_FFR.h"
/// @brief FFR 単位で DTPG の基本的な処理を行うクラス
///
/// FFR 内の故障のFFRのrootまでの故障伝搬条件は single literal の積で
/// 表されるので，このクラスではFFRのrootから外部出力までの故障伝搬条件
/// を最初に作っておく．
//////////////////////////////////////////////////////////////////////
class DtpgEngine_FFR :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  DtpgEngine_FFR(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgFFR* ffr,	           ///< [in] 故障伝搬の起点となる FFR
    const JsonValue& option        ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgEngine_FFR();


private:
  //////////////////////////////////////////////////////////////////////
  // DtpgEngine の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬の起点ノードを返す．
  const TpgNode*
  fault_origin(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

  /// @brief 故障の活性化条件
  AssignList
  fault_condition(
    const TpgFault* fault ///< [in] 対象の故障
  ) override;

};

END_NAMESPACE_DRUID

#endif // DTPGENGINE_FFR_H
