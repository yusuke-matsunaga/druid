#ifndef GATETYPE_PPI_H
#define GATETYPE_PPI_H

/// @file GateType.h
/// @brief GateType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateType_PPI GateType.h "GateType.h"
/// @brief PPI タイプの GateType
//////////////////////////////////////////////////////////////////////
class GateType_PPI :
  public GateType
{
public:

  /// @brief コンストラクタ
  GateType_PPI(
    SizeType id ///< [in] ID番号
  ) : GateType(id)
  {
  }

  /// @brief デストラクタ
  ~GateType_PPI() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief PPI のときに true を返す．
  bool
  is_ppi() const override;

};

END_NAMESPACE_DRUID

#endif // GATETYPE_PPI_H
