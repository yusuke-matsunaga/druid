#ifndef AIGGEN_H
#define AIGGEN_H

/// @file AigGen.h
/// @brief AigGen のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/Expr.h"
#include "ym/AigMgr.h"
#include "ym/AigHandle.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AigGen AigGen.h "AigGen.h"
/// @brief Expr のリストを AIG に変換するクラス
//////////////////////////////////////////////////////////////////////
class AigGen
{
public:

  /// @brief コンストラクタ
  AigGen();

  /// @brief デストラクタ
  virtual
  ~AigGen() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr のリストを AIG に変換する．
  std::vector<AigHandle>
  conv(
    AigMgr& mgr,                        ///< [in] AIGマネージャ
    const std::vector<Expr>& expr_list, ///< [in] 式のリスト
  );

};

END_NAMESPACE_DRUID

#endif // AIGGEN_H
