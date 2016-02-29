#ifndef TPGLOGICSIMPLE_H
#define TPGLOGICSIMPLE_H

/// @file TpgLogicN.h
/// @brief TpgLogicN のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgLogicN.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgLogicN TpgLogicN.h "TpgLogicN.h"
/// @brief プリミティブゲートタイプの TpgLogicN
//////////////////////////////////////////////////////////////////////
class TpgLogicSimple :
  public TpgLogicN
{
public:

  /// @brief コンストラクタ
  TpgLogicSimple();

  /// @brief デストラクタ
  ~TpgLogicSimple();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM_SATPG

#endif // TPGLOGICSIMPLE_H