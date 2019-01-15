#ifndef TPGDFFINPUT_H
#define TPGDFFINPUT_H

/// @file TpgDffInput.h
/// @brief TpgDffInput のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgPPO.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffInput TpgDffInput.h "TpgDffInput.h"
/// @brief DFFの入力を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffInput :
  public TpgPPO
{
  friend class TpgNodeFactory;

private:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] output_id 出力番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] fanin ファンインのノード
  TpgDffInput(int id,
	      int output_id,
	      const TpgDff* dff,
	      TpgNode* fanin);

  /// @brief デストラクタ
  ~TpgDffInput();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF の入力に接続している外部出力タイプの時 true を返す．
  virtual
  bool
  is_dff_input() const override;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  const TpgDff*
  dff() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF
  const TpgDff* mDff;

};

END_NAMESPACE_DRUID

#endif // TPGDFFINPUT_H
