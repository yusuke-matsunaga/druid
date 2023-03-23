#ifndef TPGDFFCONTROL_H
#define TPGDFFCONTROL_H

/// @file TpgDffControl.h
/// @brief TpgDffControl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNode.h"
#include "TpgDFF.h"


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
  TpgDffControl(
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffControl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 接続している DFF を返す．
  ///
  /// is_dff_input() | is_dff_output() | is_dff_clock() | is_dff_clear() | is_dff_preset()
  /// の時に意味を持つ．
  SizeType
  dff_id() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対応する DFF 番号
  SizeType mDffId;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgDffClock TpgDffClock.h "TpgDffClock.h"
/// @brief DFFのクロックを表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffClock :
  public TpgDffControl
{
public:

  /// @brief コンストラクタ
  TpgDffClock(
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
    const TpgNode* fanin /// @param[in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffClock() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のクロック端子に接続している出力タイプの時 true を返す．
  bool
  is_dff_clock() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgDffClear TpgDffClear.h "TpgDffClear.h"
/// @brief DFFのクリア端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffClear :
  public TpgDffControl
{
public:

  /// @brief コンストラクタ
  TpgDffClear(
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffClear() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のクリア端子に接続している力タイプの時 true を返す．
  bool
  is_dff_clear() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgDffPreset TpgDffPreset.h "TpgDffPreset.h"
/// @brief DFFのプリセット端子を表すクラス
//////////////////////////////////////////////////////////////////////
class TpgDffPreset :
  public TpgDffControl
{
public:

  /// @brief コンストラクタ
  TpgDffPreset(
    SizeType dff_id,     ///< [in] 接続しているDFFのID番号
    const TpgNode* fanin ///< [in] ファンインのノード
  );

  /// @brief デストラクタ
  ~TpgDffPreset() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief DFF のプリセット端子に接続している出力タイプの時 true を返す．
  bool
  is_dff_preset() const override;

};

END_NAMESPACE_DRUID

#endif // TPGDFFCONTROL_H
