#ifndef FAULTGROUP_H
#define FAULTGROUP_H

/// @file FaultGroup.h
/// @brief FaultGroup のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "types/AssignList.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FaultGroup FaultGroup.h "FaultGroup.h"
/// @brief パタン圧縮用の故障グループを表すクラス
///
/// - 故障のリストと必要条件(拡張テストキューブ)を持つ．
//////////////////////////////////////////////////////////////////////
class FaultGroup
{
public:

  /// @brief 空のコンストラクタ
  FaultGroup() = default;

  /// @brief 内容を指定したコンストラクタ
  FaultGroup(
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    const AssignList& gtc           ///< [in] 拡張テストキューブ
  ) : mFaultList{fault_list},
      mGTC{gtc}
  {
  }

  /// @brief 故障リストとテストベクタから故障グループを作る．
  static
  FaultGroup
  make(
    const TpgFaultList& fault_list, ///< [in] 故障のリスト
    const TestVector& tv,           ///< [in] テストベクタ
    const ConfigParam& option       ///< [in] オプション
  );

  /// @brief デストラクタ
  ~FaultGroup() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のリストを得る．
  const TpgFaultList&
  fault_list() const
  {
    return mFaultList;
  }

  /// @brief 拡張テストキューブを得る．
  const AssignList&
  gtc() const
  {
    return mGTC;
  }

  /// @brief 故障を追加する．
  void
  add(
    const TpgFault& fault, ///< [in] 追加する故障
    const AssignList& cond ///< [in] 追加する条件
  )
  {
    mFaultList.push_back(fault);
    mGTC.merge(cond);
  }

  /// @brief 内容を出力する．
  void
  print(
    std::ostream& s ///< [in] 出力ストリーム
  ) const
  {
    s << "faults:";
    for ( auto fault: fault_list() ) {
      s << " " << fault.str();
    }
    s << std::endl
      << "GTC: " << gtc() << std::endl;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 故障のリスト
  TpgFaultList mFaultList;

  // 拡張テストキューブ
  AssignList mGTC;

};

END_NAMESPACE_DRUID

#endif // FAULTGROUP_H
