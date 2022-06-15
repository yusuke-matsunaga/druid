#ifndef VIDMAP_H
#define VIDMAP_H

/// @file VidMap.h
/// @brief VidMap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "structenc_nsdef.h"
#include "TpgNode.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class VidMap VidMap.h "VidMap.h"
/// @brief ノードに関連した変数番号を返すクラス
///
/// 機能的にはノード番号をキーにした連想配列だが
/// ノード番号は連続しているのでただの配列で実装する．
//////////////////////////////////////////////////////////////////////
class VidMap
{
public:

  /// @brief コンストラクタ
  VidMap(
    int max_id = 0 ///< [in] ノード番号の最大値
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
  /// @param[in] node 対象のノード
  SatLiteral
  operator()(const TpgNode* node) const
  {
    ASSERT_COND( node->id() < mVidArray.size() );
    return mVidArray[node->id()];
  }

  /// @brief 初期化する．
  /// @param[in] max_id ノード番号の最大値
  void
  init(int max_id)
  {
    mVidArray.clear();
    mVidArray.resize(max_id);
  }

  /// @brief ノードに関連した変数番号を設定する．
  /// @param[in] node 対象のノード
  /// @param[in] vid 変数リテラル
  void
  set_vid(const TpgNode* node,
	  SatLiteral vid)
  {
    ASSERT_COND( node->id() < mVidArray.size() );
    mVidArray[node->id()] = vid;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数リテラルを格納する配列
  vector<SatLiteral> mVidArray;

};


#if 0
//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] max_id ノード番号の最大値
inline
VidMap::VidMap(int max_id) :
  mVidArray(max_id, -1)
{
}

// @brief デストラクタ
inline
VidMap::~VidMap()
{
}

// @brief ノードに関連した変数番号を返す．
// @param[in] node 対象のノード
inline
int
VidMap::operator()(const TpgNode* node) const
{
  ASSERT_COND( node->id() < mVidArray.size() );
  return mVidArray[node->id()];
}

// @brief 初期化する．
// @param[in] max_id ノード番号の最大値
inline
void
VidMap::init(int max_id)
{
  mVidArray.clear();
  mVidArray.resize(max_id);
}

// @brief ノードに関連した変数番号を設定する．
// @param[in] node 対象のノード
// @param[in] vid 変数番号
inline
void
VidMap::set_vid(const TpgNode* node,
		int vid)
{
  ASSERT_COND( node->id() < mVidArray.size() );
  mVidArray[node->id()] = vid;
}
#endif

END_NAMESPACE_DRUID

#endif // VIDMAP_H
