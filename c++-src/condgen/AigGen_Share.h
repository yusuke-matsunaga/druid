#ifndef AIGGEN_SHARE_H
#define AIGGEN_SHARE_H

/// @file AigGen_Share.h
/// @brief AigGen_Share のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "AigGen.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class AigGen_Share AigGen_Share.h "AigGen_Share.h"
/// @brief 同型のノードを共有して AIG に変換する AigGen
//////////////////////////////////////////////////////////////////////
class AigGen_Share :
  public AigGen
{
public:

  /// @brief コンストラクタ
  AigGen_Share() = default;

  /// @brief デストラクタ
  ~AigGen_Share() = default;


protected:
  //////////////////////////////////////////////////////////////////////
  // AigGen の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr のリストから AIG を作る．
  vector<AigHandle>
  conv(
    AigMgr& mgr,                  ///< [in] AIGマネージャ
    const vector<Expr>& expr_list ///< [in] 式のリスト
  ) override;

};

END_NAMESPACE_DRUID

#endif // AIGGEN_SHARE_H
