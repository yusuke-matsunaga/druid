#ifndef TPGOUTPUT_H
#define TPGOUTPUT_H

/// @file TpgOutput.h
/// @brief TpgOutput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgPPO.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgOutput TpgOutput.h "TpgOutput.h"
/// @brief 出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgOutput :
  public TpgPPO
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgOutput(
    int output_id,       ///< [in] 出力番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgOutput() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部出力タイプの時 true を返す．
  bool
  is_primary_output() const override;

};

END_NAMESPACE_DRUID

#endif // TPGOUTPUT_H
