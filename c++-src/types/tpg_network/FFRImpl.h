#ifndef FFRIMPL_H
#define FFRIMPL_H

/// @file FFRImpl.h
/// @brief FFRImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class FFRImpl FFRImpl.h "FFRImpl.h"
/// @brief TpgFFR の実装クラス
//////////////////////////////////////////////////////////////////////
class FFRImpl
{
  friend class TpgNetworkImpl;

public:

  /// @brief コンストラクタ
  FFRImpl() = default;

  /// @brief デストラクタ
  ~FFRImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const { return mId; }

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const { return mRoot; }

  /// @brief 葉のノードの配列を返す．
  const vector<const TpgNode*>&
  input_list() const { return mInputList; }

  /// @brief このFFRに含まれるノードのリストを返す．
  const vector<const TpgNode*>&
  node_list() const { return mNodeList; }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 根のノード
  const TpgNode* mRoot{nullptr};

  // 葉のノードの配列
  vector<const TpgNode*> mInputList;

  // ノードの配列
  vector<const TpgNode*> mNodeList;

};

END_NAMESPACE_DRUID

#endif // FFRIMPL_H
