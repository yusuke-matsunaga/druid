#ifndef GATETYPE_PPO_H
#define GATETYPE_PPO_H

/// @file GateType.h
/// @brief GateType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "GateType.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class GateType_PPO GateType_PPO.h "GateType_PPO.h"
/// @brief PPO タイプの GateType
//////////////////////////////////////////////////////////////////////
class GateType_PPO :
  public GateType
{
public:

  /// @brief コンストラクタ
  GateType_PPO(
    SizeType id ///< [in] ID番号
  ) : GateType(id)
  {
  }

  /// @brief デストラクタ
  ~GateType_PPO() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力数を返す．
  SizeType
  input_num() const override;

  /// @brief PPO のときに true を返す．
  bool
  is_ppo() const override;

};

END_NAMESPACE_DRUID

#endif // GATETYPE_PPO_H
