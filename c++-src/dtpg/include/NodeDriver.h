#ifndef NODEDRIVER_H
#define NODEDRIVER_H

/// @file NodeDriver.h
/// @brief NodeDriver のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "DtpgDriver.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class NodeDriver NodeDriver.h "NodeDriver.h"
/// @brief ノード単位で DTPG の基本的な処理を行うクラス
///
/// 各故障のノード内の故障伝搬条件は single literal の積で表されるので
/// SATソルバの節として生成する必要はない．
/// そのため，ノードの出力から外部出力までの故障伝搬条件だけを作っておく．
//////////////////////////////////////////////////////////////////////
class NodeDriver :
  public DtpgDriver
{
public:

  /// @brief コンストラクタ
  NodeDriver(
    DtpgMgr& mgr,           ///< [in] DTPGマネージャ
    const TpgNode* node,    ///< [in] 故障伝搬の起点となるノード
    const JsonValue& option ///< [in] オプション
  );

  /// @brief デストラクタ
  ~NodeDriver() = default;

};

END_NAMESPACE_DRUID

#endif // NODEDRIVER_H
