#ifndef TPGDFFCONTROL_H
#define TPGDFFCONTROL_H

/// @file TpgDffControl.h
/// @brief TpgDffControl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @class TpgDffControl TpgDffControl.h "TpgDffControl.h"
/// @brief DFFの制御端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffControl :
  public TpgNode
{
public:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] dff 接続しているDFF
  TpgDffControl(ymuint id,
		TpgDff* dff,
		TpgNode* fanin);

  /// @brief デストラクタ
  ~TpgDffControl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  virtual
  ymuint
  fanin_num() const;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  virtual
  TpgNode*
  fanin(ymuint pos) const;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  TpgDff*
  dff() const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンイン
  TpgNode* mFanin;

  // 対応する DFF
  TpgDff* mDff;

};

END_NAMESPACE_YM_SATPG

#endif // TPGDFFCONTROL_H
