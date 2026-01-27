#ifndef JUSTNAIVE_H
#define JUSTNAIVE_H

/// @file JustNaive.h
/// @brief JustNaive のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "Justifier.h"


BEGIN_NAMESPACE_DRUID

class JustData;

//////////////////////////////////////////////////////////////////////
/// @class JustNaive JustNaive.h "td/JustNaive.h"
/// @brief 関係するすべて値割り当てを記録する JustImpl
//////////////////////////////////////////////////////////////////////
class JustNaive :
  public Justifier
{
public:

  /// @brief コンストラクタ
  JustNaive(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  );

  /// @brief デストラクタ
  ~JustNaive();


private:
  //////////////////////////////////////////////////////////////////////
  // JustImpl の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief justify の実際の処理
  AssignList
  _justify(
    const AssignList& assign_list ///< [in] 割当リスト
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 値を取り出す．
  bool
  get_bval(
    const TpgNode& node, ///< [in] 対象のノード
    int time             ///< [in] 時刻(0 or 1)
  );

};

END_NAMESPACE_DRUID

#endif // JUSTNAIVE_H
