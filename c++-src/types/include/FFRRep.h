#ifndef FFR_REP_H
#define FFR_REP_H

/// @file FFRRep.h
/// @brief FFRRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class NodeRep;

//////////////////////////////////////////////////////////////////////
/// @class FFRRep FFRRep.h "FFRRep.h"
/// @brief TpgFFR の本体
//////////////////////////////////////////////////////////////////////
class FFRRep
{
public:

  /// @brief コンストラクタ
  FFRRep(
    SizeType id,                                   ///< [in] ID番号
    const NodeRep* root,                           ///< [in] 根のノード
    const std::vector<const NodeRep*>& input_list, ///< [in] 葉のノードのリスト
    const std::vector<const NodeRep*>& node_list   ///< [in] 含まれるノードのリスト
  ) : mId{id},
      mRoot{root},
      mInputList{input_list},
      mNodeList{node_list}
  {
  }

  /// @brief デストラクタ
  ~FFRRep() = default;


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
  const NodeRep*
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
  const NodeRep*
  input(
    SizeType index ///< [in] 位置番号 ( 0 <= index < input_num() )
  ) const
  {
    if ( index >= input_num() ) {
      throw std::out_of_range{"index is out of range"};
    }
    return mInputList[index];
  }

  /// @brief 葉(FFRの入力)のリストを返す．
  const std::vector<const NodeRep*>&
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
  const NodeRep*
  node(
    SizeType index ///< [in] 位置番号 ( 0 <= index < node_num() )
  ) const
  {
    if ( index >= node_num() ) {
      throw std::out_of_range{"index is out of range"};
    }
    return mNodeList[index];
  }

  /// @brief このFFRに含まれるノードのリストを返す．
  const std::vector<const NodeRep*>&
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
  const NodeRep* mRoot{nullptr};

  // 葉のノードの配列
  std::vector<const NodeRep*> mInputList;

  // ノードのリスト
  std::vector<const NodeRep*> mNodeList;

};

END_NAMESPACE_DRUID

#endif // FFR_REP_H
