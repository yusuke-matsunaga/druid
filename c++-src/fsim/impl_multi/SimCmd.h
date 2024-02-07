#ifndef SIMCMD2_H
#define SIMCMD2_H

/// @file SimCmd2.h
/// @brief SiCmd2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimCmd2 SimCmd.h "SimCmd.h"
/// @brief sppfp 用の SimCmd
//////////////////////////////////////////////////////////////////////
class SimCmd2 :
  public SimCmd
{
public:

  /// @brief コンストラクタ
  SimCmd2(
    SizeType id,                            ///< [in] コマンド番号
    const vector<const SimNode*>& node_list ///< [in] ノードのリスト
  ) : mId{id},
      mNodeList{node_list}
  {
  }

  /// @brief デストラクタ
  ~SimCmd2() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief コマンド番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief ノードリストを返す．
  const vector<const SimNode*>&
  node_list() const
  {
    return mNodeList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コマンド番号
  SizeType mId;

  // ノードのリスト
  vector<const SimNode*> mNodeList;

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMCMD_H
