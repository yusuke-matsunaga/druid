#ifndef DOMCHECKER_H
#define DOMCHECKER_H

/// @file DomChecker.h
/// @brief DomChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "dtpg/FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class DomChecker DomChecker.h "DomChecker.h"
/// @brief ２つの FFR の故障の間の支配関係を調べるクラス
///
/// 結果は引数の fault_info に格納される．
//////////////////////////////////////////////////////////////////////
class DomChecker
{
public:

  /// @brief 統計情報を表す構造体
  struct Stats {
    SizeType check_count{0};
    SizeType dom1_count{0};
    SizeType dom2_count{0};

    Stats
    operator+(
      const Stats& right
    ) const
    {
      return Stats{check_count + right.check_count,
		   dom1_count + right.dom1_count,
		   dom2_count + right.dom2_count};
    }

    Stats&
    operator+=(
      const Stats& right
    )
    {
      check_count += right.check_count;
      dom1_count += right.dom1_count;
      dom2_count += right.dom2_count;
      return *this;
    }
  };


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ２つのFFRの故障の間の支配関係を調べる．
  static
  Stats
  run(
    const TpgFFR& ffr1,              ///< [in] FFR1
    const TpgFFR& ffr2,              ///< [in] FFR2
    const TpgFaultList& fault_list1, ///< [in] ffr1 の故障のリスト
    const TpgFaultList& fault_list2, ///< [in] ffr2 の故障のリスト
    FaultInfo& fault_info,           ///< [in] 結果を格納するオブジェクト
    const ConfigParam& option = {}   ///< [in] オプション
  );

};

END_NAMESPACE_DRUID

#endif // DOMCHECKER_H
