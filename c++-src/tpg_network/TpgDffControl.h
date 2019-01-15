#ifndef TPGDFFCONTROL_H
#define TPGDFFCONTROL_H

/// @file TpgDffControl.h
/// @brief TpgDffControl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNode.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class TpgDffControl TpgDffControl.h "TpgDffControl.h"
/// @brief DFFの制御端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffControl :
  public TpgNode
{
protected:

  /// @brief コンストラクタ
  /// @param[in] id ID番号
  /// @param[in] dff 接続しているDFF
  TpgDffControl(int id,
		const TpgDff* dff,
		TpgNode* fanin);

  /// @brief デストラクタ
  virtual
  ~TpgDffControl();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインのリストを得る．
  virtual
  Array<const TpgNode*>
  fanin_list() const override;

  /// @brief ファンイン数を得る．
  virtual
  int
  fanin_num() const override;

  /// @brief ファンインを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < fanin_num() )
  virtual
  const TpgNode*
  fanin(int pos) const override;

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  virtual
  const TpgDff*
  dff() const override;


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
  const TpgDff* mDff;

};

END_NAMESPACE_DRUID

#endif // TPGDFFCONTROL_H
