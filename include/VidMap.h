#ifndef VIDMAP_H
#define VIDMAP_H

/// @file VidMap.h
/// @brief VidMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "structenc_nsdef.h"
#include "TpgNode.h"
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
  ) : mVidArray(max_id, kSatLiteralX)
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
    const TpgNode* node ///< [in] 対象のノード
  ) const
  {
    ASSERT_COND( node->id() < mVidArray.size() );
    return mVidArray[node->id()];
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
    const TpgNode* node, ///< [in] 対象のノード
    SatLiteral vid       ///< [in] 変数リテラル
  )
  {
    ASSERT_COND( node->id() < mVidArray.size() );
    mVidArray[node->id()] = vid;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数リテラルを格納する配列
  vector<SatLiteral> mVidArray;

};

END_NAMESPACE_DRUID

#endif // VIDMAP_H
