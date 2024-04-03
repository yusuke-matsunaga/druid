#ifndef TPGMFFC_H
#define TPGMFFC_H

/// @file TpgMFFC.h
/// @brief TpgMFFC のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "TpgFFR.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgMFFC TpgMFFC.h "TpgMFFC.h"
/// @brief TpgNetwork の MFFC(Maximal Fanout Free Cone) の情報を表すクラス
/// @sa TpgNetwork
/// @sa TpgNode
/// @sa TpgFFR
///
/// 具体的には以下の情報を持つ．
/// - MFFC の根のノード
/// - MFFC に含まれる FFR のリスト
/// - MFFC に含まれる故障のリスト
/// 一度設定されたら不変のオブジェクトとなる．
//////////////////////////////////////////////////////////////////////
class TpgMFFC
{
public:

  /// @brief コンストラクタ
  TpgMFFC(
    SizeType id,                          ///< [in] ID番号
    const TpgNode* root,                  ///< [in] 根のノード
    const vector<const TpgFFR*>& ffr_list ///< [in] FFRのリスト
  ) : mId{id},
      mRoot{root},
      mFFRList{ffr_list}
  {
  }

  /// @brief デストラクタ
  ~TpgMFFC() = default;


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

  /// @brief このMFFCに含まれるFFR数を返す．
  SizeType
  ffr_num() const
  {
    return mFFRList.size();
  }

  /// @brief このMFFCに含まれるFFRを返す．
  const TpgFFR*
  ffr(
    SizeType pos /// [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const
  {
    ASSERT_COND( pos < ffr_num() );

    return mFFRList[pos];
  }

  /// @brief このMFFCに含まれるFFRのリストを返す．
  const vector<const TpgFFR*>&
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
  const TpgNode* mRoot{nullptr};

  // FFRのリスト
  vector<const TpgFFR*> mFFRList;

};

END_NAMESPACE_DRUID

#endif // TPGMFFC_H
