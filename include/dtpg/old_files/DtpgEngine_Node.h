#ifndef DTPGENGINE_NODE_H
#define DTPGENGINE_NODE_H

/// @file DtpgEngine_Node.h
/// @brief DtpgEngine_Node のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgEngine.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DtpgEngine_Node DtpgEngine_Node.h "DtpgEngine_Node.h"
/// @brief ノード単位で DTPG の基本的な処理を行うクラス
///
/// ノードの出力までの故障伝搬条件は single literal の積で
/// 表されるので，このクラスではノードの出力から外部出力までの
/// 故障伝搬条件を最初に作っておく．
//////////////////////////////////////////////////////////////////////
class DtpgEngine_Node :
  public DtpgEngine
{
public:

  /// @brief コンストラクタ
  DtpgEngine_Node(
    const TpgNetwork& network,     ///< [in] 対象のネットワーク
    const TpgNode* node,           ///< [in] 故障のあるノード
    const JsonValue& option        ///< [in] オプション
  );

  /// @brief デストラクタ
  ~DtpgEngine_Node();


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

#endif // DTPGENGINE_NODE_H
