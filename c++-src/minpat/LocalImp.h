#ifndef LOCALIMP_H
#define LOCALIMP_H

/// @file LocalImp.h
/// @brief LocalImp のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <deque>
#include "druid.h"
#include "TpgNetwork.h"
#include "NodeTimeValList.h"
#include "Val3.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class LocalImp LocalImp.h "LocalImp.h"
/// @brief 構造に基づいて含意操作を行うクラス
//////////////////////////////////////////////////////////////////////
class LocalImp
{
public:

  /// @brief コンストラクタ
  LocalImp(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) : mNetwork{network},
      mValArray{vector<Val3>(network.node_num(), Val3::_X),
		vector<Val3>(network.node_num(), Val3::_X)}
  {
  }

  /// @brief デストラクタ
  ~LocalImp() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 含意操作を行う．
  NodeTimeValList
  run(
    const NodeTimeValList& assignments ///< [in] 割り当て
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 前方含意を行う．
  void
  forward_imp(
    const TpgNode* node,
    int time
  );

  /// @brief 後方含意を行う．
  void
  backward_imp(
    const TpgNode* node,
    int time
  );

  /// @brief ノードの値を読み出す．
  Val3
  get_val(
    const TpgNode* node,
    int time
  );

  /// @brief ノードに値を割り当てる．
  void
  assign(
    const TpgNode* node,
    int time,
    bool val
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ネットワーク
  const TpgNetwork& mNetwork;

  // 値の配列
  std::vector<Val3> mValArray[2];

  // キュー
  std::deque<NodeTimeVal> mQueue;

  // 結果の割り当てリスト
  NodeTimeValList mAssignList;

};

END_NAMESPACE_DRUID

#endif // LOCALIMP_H
