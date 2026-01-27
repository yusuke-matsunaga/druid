#ifndef MFFC_REP_H
#define MFFC_REP_H

/// @file MFFCRep.h
/// @brief MFFCRep のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class NodeRep;
class FFRRep;

//////////////////////////////////////////////////////////////////////
/// @class MFFCRep MFFCRep.h "MFFCRep.h"
/// @brief TpgMFFC の本体
//////////////////////////////////////////////////////////////////////
class MFFCRep
{
public:

  /// @brief コンストラクタ
  MFFCRep(
    SizeType id,                                ///< [in] ID番号
    const NodeRep* root,                       ///< [in] 根のノード
    const std::vector<const FFRRep*>& ffr_list ///< [in] FFRのリスト
  ) : mId{id},
      mRoot{root},
      mFFRList{ffr_list}
  {
  }

  /// @brief デストラクタ
  ~MFFCRep() = default;


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

  /// @brief このMFFCに含まれるFFR数を返す．
  SizeType
  ffr_num() const
  {
    return mFFRList.size();
  }

  /// @brief このMFFCに含まれるFFRを返す．
  const FFRRep*
  ffr(
    SizeType index /// [in] 位置番号 ( 0 <= index < ffr_num() )
  ) const
  {
    if ( index >= ffr_num() ) {
      throw std::out_of_range{"index is out of range"};
    }
    return mFFRList[index];
  }

  /// @brief このMFFCに含まれるFFRのリストを返す．
  const std::vector<const FFRRep*>&
  ffr_list() const
  {
    return mFFRList;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID番号
  SizeType mId;

  // 根のノード
  const NodeRep* mRoot{nullptr};

  // FFRのリスト
  std::vector<const FFRRep*> mFFRList;

};

END_NAMESPACE_DRUID

#endif // MFFC_REP_H
