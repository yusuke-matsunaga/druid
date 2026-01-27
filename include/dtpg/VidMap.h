#ifndef VIDMAP_H
#define VIDMAP_H

/// @file VidMap.h
/// @brief VidMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/TpgNode.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class VidMap VidMap.h "VidMap.h"
/// @brief ノードに関連した変数リテラルを返すクラス
///
/// 機能的にはノード番号をキーにした連想配列だが
/// ノード番号は連続しているのでただの配列で実装する．
//////////////////////////////////////////////////////////////////////
class VidMap
{
public:

  /// @brief コンストラクタ
  VidMap(
    SizeType max_id = 0 ///< [in] ノード番号の最大値
  ) : mVidArray(max_id, SatLiteral::X)
  {
  }

  /// @brief デストラクタ
  ~VidMap() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードに関連した変数リテラルを返す．
  SatLiteral
  operator()(
    const TpgNode& node ///< [in] 対象のノード
  ) const
  {
    _check_node_id(node);
    return mVidArray[node.id()];
  }

  /// @brief 初期化する．
  void
  init(
    SizeType max_id ///< [in] ノード番号の最大値
  )
  {
    mVidArray.clear();
    mVidArray.resize(max_id);
  }

  /// @brief ノードに関連した変数番号を設定する．
  void
  set_vid(
    const TpgNode& node, ///< [in] 対象のノード
    SatLiteral vid       ///< [in] 変数リテラル
  )
  {
    _check_node_id(node);
    mVidArray[node.id()] = vid;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード番号をチェックする．
  void
  _check_node_id(
    const TpgNode& node
  ) const
  {
    if ( node.id() >= mVidArray.size() ) {
      throw std::out_of_range{"node.id() is out of range"};
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数リテラルを格納する配列
  std::vector<SatLiteral> mVidArray;

};

END_NAMESPACE_DRUID

#endif // VIDMAP_H
