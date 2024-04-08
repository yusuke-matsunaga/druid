#ifndef TPGFFR_H
#define TPGFFR_H

/// @file TpgFFR.h
/// @brief TpgFFR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgFFR TpgFFR.h "TpgFFR.h"
/// @brief TpgNetwork の FFR の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgMFFC
///
/// 具体的には以下の情報を持つ．
/// - FFR の根のノード
/// - FFR の葉のノードのリスト
/// - FFR に含まれる全ノードのリスト
/// - ffR に含まれる故障のリスト
/// 一度設定された不変のオブジェクトとなる．
///
/// FFR の葉のノードはそのFFRには含まれない．
//////////////////////////////////////////////////////////////////////
class TpgFFR
{
public:

  /// @brief コンストラクタ
  TpgFFR(
    SizeType id,                              ///< [in] ID番号
    const TpgNode* root,                      ///< [in] 根のノード
    const vector<const TpgNode*>& input_list, ///< [in] 葉のノードのリスト
    const vector<const TpgNode*>& node_list   ///< [in] 含まれるノードのリスト
  ) : mId{id},
      mRoot{root},
      mInputList{input_list},
      mNodeList{node_list}
  {
  }

  /// @brief デストラクタ
  ~TpgFFR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 根のノードを返す．
  const TpgNode*
  root() const
  {
    return mRoot;
  }

  /// @brief 葉(FFRの入力)の数を返す．
  SizeType
  input_num() const
  {
    return mInputList.size();
  }

  /// @brief 葉(FFRの入力)を返す．
  const TpgNode*
  input(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < input_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < input_num() );

    return mInputList[pos];
  }

  /// @brief 葉(FFRの入力)のリストを返す．
  const vector<const TpgNode*>&
  input_list() const
  {
    return mInputList;
  }

  /// @brief このFFRに含まれるノード数を返す．
  SizeType
  node_num() const
  {
    return mNodeList.size();
  }

  /// @brief このFFRに含まれるノードを返す．
  const TpgNode*
  node(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < node_num() )
  ) const
  {
    ASSERT_COND( 0 <= pos && pos < node_num() );

    return mNodeList[pos];
  }

  /// @brief このFFRに含まれるノードのリストを返す．
  const vector<const TpgNode*>&
  node_list() const
  {
    return mNodeList;
  }


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

  // ノードのリスト
  vector<const TpgNode*> mNodeList;

};

END_NAMESPACE_DRUID

#endif // TPGFFR_H
