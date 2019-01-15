#ifndef TPGDFFOUTPUT_H
#define TPGDFFOUTPUT_H

/// @file TpgDffOutput.h
/// @brief TpgDffOutput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgPPI.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffOutput TpgDffOutput.h "TpgDffOutput.h"
/// @brief DFF の出力ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffOutput :
  public TpgPPI
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] input_id 入力番号
  /// @param[in] dff 接続しているDFF
  TpgDffOutput(int id,
	       int input_id,
	       const TpgDff* dff);

  /// @brief デストラクタ
  virtual
  ~TpgDffOutput();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF の出力に接続している外部入力タイプの時 true を返す．
  virtual
  bool
  is_dff_output() const override;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  const TpgDff*
  dff() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF
  const TpgDff* mDff;

};

END_NAMESPACE_DRUID

#endif // TPGDFFOUTPUT_H
