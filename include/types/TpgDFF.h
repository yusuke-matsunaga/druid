#ifndef TPGDFF_H
#define TPGDFF_H

/// @file TpgDFF.h
/// @brief TpgDFF のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class DFFImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgDFF TpgDFF.h "TpgDFF.h"
/// @brief DFF を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDFF
{
public:

  /// @brief コンストラクタ
  TpgDFF(
    const DFFImpl* impl = nullptr ///< [in] 実装オブジェクト
  ) : mImpl{impl}
  {
  }

  /// @brief デストラクタ
  ~TpgDFF() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const;

  /// @brief 入力端子のノードを返す．
  const TpgNode*
  input() const;

  /// @brief 出力端子のノードを返す．
  const TpgNode*
  output() const;

  /// @brief クリア端子のノードを返す．
  const TpgNode*
  clear() const;

  /// @brief プリセット端子のノードを返す．
  const TpgNode*
  preset() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装オブジェクト
  const DFFImpl* mImpl{nullptr};

};

END_NAMESPACE_DRUID

#endif // TPGDFF_H
