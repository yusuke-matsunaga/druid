#ifndef TPGINPUT_H
#define TPGINPUT_H

/// @file TpgInput.h
/// @brief TpgInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgPPI.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgInput TpgInput.h "TpgInput.h"
/// @brief 入力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgInput :
  public TpgPPI
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] input_id 入力番号
  /// @param[in] fanout_num ファンアウト数
  TpgInput(ymuint id,
	   ymuint input_id,
	   ymuint fanout_num);

  /// @brief デストラクタ
  virtual
  ~TpgInput();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 外部入力タイプの時 true を返す．
  virtual
  bool
  is_primary_input() const;


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

#endif // TPGINPUT_H
