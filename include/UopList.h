#ifndef UOPLIST_H
#define UOPLIST_H

/// @file UopList.h
/// @brief UopList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "UntestOp.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class UopList UopList.h "UopList.h"
/// @brief UntestOp のリストを持つクラス
///
/// 自身も UntestOp の継承クラスである．
//////////////////////////////////////////////////////////////////////
class UopList :
  public UntestOp
{
public:

  /// @brief コンストラクタ
  UopList();

  /// @brief デストラクタ
  ///
  /// ここに登録された UntestOp も破棄される．
  virtual
  ~UopList();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief uop を追加する．
  void
  add(UntestOp* uop);

  /// @brief テスト不能故障と判定された時の処理
  /// @param[in] f 故障
  virtual
  void
  operator()(const TpgFault* f);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // uop のリスト
  vector<UntestOp*> mUopList;

};

END_NAMESPACE_DRUID

#endif // UOPLIST_H
