#ifndef BSINFO_H
#define BSINFO_H

/// @file BSInfo.h
/// @brief BSInfo のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ConeInfo.h"
#include "TpgNetwork.h"
#include "TpgNodeSet.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BSInfo BSInfo.h "BSInfo.h"
/// @brief Broad Side方式の1時刻前の構造情報
///
/// メンバは初期化されたら不変
//////////////////////////////////////////////////////////////////////
class BSInfo
{
public:

  /// @brief コンストラクタ
  BSInfo(
    const ConeInfo& cone_info ///< [in] 故障コーンの情報
  ) : mNetwork{cone_info.network()}
  {
    // TFIコーンに含まれるDFFの出力ノードを求める．
    vector<const TpgNode*> tmp_list;
    for ( auto node: cone_info.dff_output_list() ) {
      tmp_list.push_back(node->alt_node());
    }
    auto root = cone_info.root();
    tmp_list.push_back(root);
    mTFIList = TpgNodeSet::get_tfi_list(mNetwork.node_num(), tmp_list);
  }

  /// @brief デストラクタ
  ~BSInfo() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 対象のネットワークを返す．
  const TpgNetwork&
  network() const
  {
    return mNetwork;
  }

  /// @brief TFI コーンのリストを返す．
  const vector<const TpgNode*>&
  tfi_list() const
  {
    return mTFIList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // TFI のノードのリスト
  vector<const TpgNode*> mTFIList;

};

END_NAMESPACE_DRUID

#endif // BSINFO_H
