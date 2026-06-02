#ifndef FILTER_H
#define FILTER_H

/// @file Filter.h
/// @brief Filter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "EqDomCand.h"
#include "FaultInfo.h"
#include "types/PackedVal.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Filter Filter.h "Filter.h"
/// @brief 等価故障と支配故障候補のフィルター
//////////////////////////////////////////////////////////////////////
class Filter
{
public:

  /// @brief コンストラクタ
  Filter(
    const FaultInfo& fault_info ///< [in] 対象の故障の情報
  ) : mFaultInfo{fault_info}
  {
  }

  /// @brief デストラクタ
  virtual
  ~Filter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価故障と支配故障の候補を求める．
  EqDomCand
  run(
    const ConfigParam& option    ///< [in] オプション
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 更新処理
  virtual
  bool
  update(
    const std::vector<PackedVal>& dpat_array ///< [in] 故障の検出状況のピットパタン
  ) = 0;

  /// @brief 終了処理
  virtual
  EqDomCand
  end() = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障のリスト
  const FaultInfo& mFaultInfo;

};

END_NAMESPACE_DRUID

#endif // FILTER_H
