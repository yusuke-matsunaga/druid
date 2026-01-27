#ifndef SIMNODELIST_H
#define SIMNODELIST_H

/// @file SimNodeList.h
/// @brief SimNodeList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimNodeList SimNodeList.h "SimNodeList.h"
/// @brief vector<SimNode*> の部分をリストのように見せかけるクラス
//////////////////////////////////////////////////////////////////////
class SimNodeList
{
public:

  using iterator = std::vector<SimNode*>::const_iterator;

public:

  /// @brief コンストラクタ
  SimNodeList(
    iterator begin, ///< [in] 開始位置の反復子
    iterator end    ///< [in] 終了位置の反復子
  ) : mBegin{begin},
      mEnd{end}
  {
  }

  /// @brief デストラクタ
  ~SimNodeList() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mEnd - mBegin;
  }

  /// @brief 先頭の反復子を返す．
  iterator
  begin() const
  {
    return mBegin;
  }

  /// @brief 末尾の反復子を返す．
  iterator
  end() const
  {
    return mEnd;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 開始位置
  iterator mBegin;

  // 終了位置
  iterator mEnd;
};

END_NAMESPACE_DRUID_FSIM

#endif // SIMNODELIST_H
