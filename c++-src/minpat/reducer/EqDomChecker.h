#ifndef EQDOMCHECKER_H
#define EQDOMCHECKER_H

/// @file EqDomChecker.h
/// @brief EqDomChecker のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgFaultList.h"
#include "EqDomMgr.h"
#include "FaultInfo.h"
#include "misc/ConfigParam.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class EqDomChecker EqDomChecker.h "EqDomChecker.h"
/// @brief 等価故障のチェックを行うクラス
///
/// 複数の結果を保持するために用いる．
//////////////////////////////////////////////////////////////////////
class EqDomChecker
{
public:

  /// @brief コンストラクタ
  EqDomChecker() = default;

  /// @brief デストラクタ
  ~EqDomChecker() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 等価故障のチェックを行う．
  void
  check_equiv(
    EqDomMgr* mgr,            ///< [in] マネージャ
    SizeType group_id,        ///< [in] グループ番号
    const ConfigParam& option ///< [in] オプション
  );

  /// @brief 支配故障のチェックを行う．
  void
  check_dominance(
    EqDomMgr* mgr,             ///< [in] マネージャ
    const TpgFault& fault,     ///< [in] 対象の故障
    const ConfigParam& option  ///< [in] オプション
  );

  /// @brief 結果の情報を更新する．
  bool
  update_results(
    SizeType& check_count,
    SizeType& success_count,
    std::vector<TestVector>& tv_list
  ) const
  {
    check_count += mCheckCount;
    success_count += mSuccessCount;
    if ( !mTvList.empty() ) {
      tv_list.insert(tv_list.end(), mTvList.begin(), mTvList.end());
    }
    return mChanged;
  }

  /// @brief 反例のテストベクタを持つとき true を返す．
  bool
  has_tv() const
  {
    return !mTvList.empty();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // チェックを行った回数
  SizeType mCheckCount{0};

  // チェックが成功した回数
  SizeType mSuccessCount{0};

  // 変化があった時 true にするフラグ
  bool mChanged{false};

  // 反例のテストベクタ
  std::vector<TestVector> mTvList;

};

END_NAMESPACE_DRUID

#endif // EQDOMCHECKER_H
