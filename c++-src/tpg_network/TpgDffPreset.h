#ifndef TPGDFFPRESET_H
#define TPGDFFPRESET_H

/// @file TpgDffPreset.h
/// @brief TpgDffPreset のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDffControl.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffPreset TpgDffPreset.h "TpgDffPreset.h"
/// @brief DFFのプリセット端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffPreset :
  public TpgDffControl
{
  friend class TpgNetworkImpl;

private:

  /// @brief コンストラクタ
  TpgDffPreset(
    TpgDFF dff,          /// @param[in] 接続しているDFFのID番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffPreset() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
  bool
  is_dff_preset() const override;

};

END_NAMESPACE_DRUID

#endif // TPGDFFPRESET_H
