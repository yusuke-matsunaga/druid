#ifndef SIMCONTEXT_H
#define SIMCONTEXT_H

/// @file SimContext.h
/// @brief SimContext のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimContext SimContext.h "SimContext.h"
/// @brief シミュレーションのコンテキストを表すクラス
//////////////////////////////////////////////////////////////////////
class SimContext
{
public:

  /// @brief コンストラクタ
  SimContext(
    const std::vector<FSIM_VALTYPE>& val_array, ///< [in] 元の値の配列
    const SizeType& timestamp              ///< [in] val_array のタイムスタンプ
  ) : mValArray{val_array},
      mTimeStamp{time_stamp}
  {
  }

  /// @brief デストラクタ
  ~SimContext() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 元の値の配列を得る．
  const std::vector<FSIM_VALTYPE>&
  val_array() const
  {
    return mValArray;
  }

  /// @brief val_array のタイムスタンプを返す．
  SizeType
  timestamp() const
  {
    return mTimeStamp;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 元の値の配列
  const std::vector<FSIM_VALTYPE>& mOrigValArray;

  // mOrigValArray のタイムスタンプ
  const SizeType& mTimeStamp;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMCONTEXT_H
