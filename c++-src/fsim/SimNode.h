#ifndef FSIM_SIMNODE_H
#define FSIM_SIMNODE_H

/// @file SimNode.h
/// @brief SimNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "TpgNode.h"
#include "PackedVal.h"
#include "PackedVal3.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SimNode SimNode.h "SimNode.h"
/// @brief 故障シミュレーション用のノード
///
/// 出力値の計算はゲートの種類によって異なるので仮想関数にしている．<br>
/// 注意が必要なのがファンアウトの情報．最初のファンアウトだけ個別のポインタで
/// 持ち，２番目以降のファンアウトは配列で保持する．これは多くのノードが
/// 一つしかファンアウトを持たず，その場合に配列を使うとメモリ参照が余分に発生する
/// ため．
//////////////////////////////////////////////////////////////////////
class SimNode
{
  friend class EventQ;

protected:

  /// @brief コンストラクタ
  SimNode(
    SizeType id ///< [in] ノード番号
  );


public:

  /// @brief コピーコンストラクタは禁止
  SimNode(const SimNode& src) = delete;

  /// @brief 代入演算子も禁止
  const SimNode&
  operator=(const SimNode& src) = delete;

  /// @brief デストラクタ
  virtual
  ~SimNode();


public:
  //////////////////////////////////////////////////////////////////////
  // 生成用のクラスメソッド
  //////////////////////////////////////////////////////////////////////

  /// @brief 入力ノードを生成するクラスメソッド
  static
  SimNode*
  new_input(
    SizeType id ///< [in] ノード番号
  );

  /// @brief 論理ノードを生成するクラスメソッド
  static
  SimNode*
  new_gate(
    SizeType id,                   ///< [in] ノード番号
    GateType type,                 ///< [in] ゲートの種類
    const vector<SimNode*>& inputs ///< [in] ファンインのノードのリスト
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の取得
  //////////////////////////////////////////////////////////////////////

  /// @brief ID番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief ゲートタイプを返す．
  virtual
  GateType
  gate_type() const = 0;

  /// @brief ファンイン数を得る．
  virtual
  SizeType
  fanin_num() const = 0;

  /// @brief pos 番めのファンインを得る．
  virtual
  SimNode*
  fanin(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanin_num() )
  ) const = 0;

  /// @brief ファンアウト数を得る．
  SizeType
  fanout_num() const
  {
    return static_cast<SizeType>(mFanoutNum >> 16);
  }

  /// @brief ファンアウトの先頭のノードを得る．
  SimNode*
  fanout_top() const
  {
    return mFanoutTop;
  }

  /// @brief 最初のファンアウト先の入力位置を得る．
  SizeType
  fanout_ipos() const
  {
    return static_cast<SizeType>((mFanoutNum >> 4) & 0x0FFFU);
  }

  /// @brief pos 番目のファンアウトを得る．
  ///
  /// ただし fanout_num() == 1 の時は使えない．
  SimNode*
  fanout(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanout_num() )
  ) const
  {
    SimNode** fanouts = reinterpret_cast<SimNode**>(mFanoutTop);
    return fanouts[pos];
  }

  /// @brief FFR の根のノードの時 true を返す．
  bool
  is_ffr_root() const
  {
    return static_cast<bool>((mFanoutNum >> 1) & 1U);
  }

  /// @brief FFR の根のノードを返す．
  SimNode*
  ffr_root()
  {
    SimNode* root = this;
    while ( !root->is_ffr_root() ) {
      root = root->fanout_top();
    }

    return root;
  }

  /// @brief レベルを得る．
  SizeType
  level() const
  {
    return mLevel;
  }

  /// @brief 出力ノードの時 true を返す．
  bool
  is_output() const
  {
    return static_cast<bool>((mFanoutNum >> 0) & 1U);
  }

  /// @brief 内容をダンプする．
  virtual
  void
  dump(
    ostream& s ///< [in] 出力ストリーム
  ) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の設定用関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力マークをつける．
  void
  set_output()
  {
    mFanoutNum |= 1U;
  }

  /// @brief ファンアウトリストを作成する．
  void
  set_fanout_list(
    const vector<SimNode*>& fo_list,
    SizeType ipos
  );

  /// @brief FFR の根の印をつける．
  void
  set_ffr_root()
  {
    mFanoutNum |= 2U;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションに関する情報の取得/設定
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値を得る．
  FSIM_VALTYPE
  val() const
  {
    return mVal;
  }

  /// @brief 出力値のセットを行う．
  void
  set_val(
    FSIM_VALTYPE val ///< [in] 値
  )
  {
    mVal = val;
  }

  /// @brief 出力値のセットを行う(マスク付き)．
  void
  set_val(
    FSIM_VALTYPE val, ///< [in] 値
    PackedVal mask    ///< [in] マスク
  )
  {
#if FSIM_VAL2
    mVal &= ~mask;
    mVal |= (val & mask);
#elif FSIM_VAL3
    mVal.set_with_mask(val, mask);
#endif
  }

  /// @brief 出力値を計算する．
  void
  calc_val()
  {
    set_val(_calc_val());
  }

  /// @brief 出力値を計算する(マスク付き)．
  ///
  /// mask で1の立っているビットだけ更新する．
  void
  calc_val(
    PackedVal mask ///< [in] マスク
  )
  {
    set_val(_calc_val(), mask);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  /// @return 計算結果を返す．
  virtual
  FSIM_VALTYPE
  _calc_val() = 0;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(
    SizeType ipos ///< [in] 入力番号
  ) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief レベルを設定する．
  void
  set_level(
    SizeType level ///< [in] レベル
  );


private:
  //////////////////////////////////////////////////////////////////////
  // EventQ 用の関数
  //////////////////////////////////////////////////////////////////////

  /// @brief キューに積まれていたら true を返す．
  bool
  in_queue() const
  {
    return static_cast<bool>((mFanoutNum >> 2) & 1U);
  }

  /// @brief キューフラグをセットする．
  void
  set_queue()
  {
    mFanoutNum |= 1U << 2;
  }

  /// @brief キューフラグをクリアする．
  void
  clear_queue()
  {
    mFanoutNum &= ~(1U << 2);
  }

  /// @brief 反転マスクを持っていたら true を返す．
  bool
  has_flip_mask() const
  {
    return static_cast<bool>((mFanoutNum >> 3) & 1U);
  }

  /// @brief 反転フラグをセットする．
  void
  set_flip()
  {
    mFanoutNum |= 1U << 3;
  }

  /// @brief 反転フラグをクリアする．
  void
  clear_flip()
  {
    mFanoutNum &= ~(1U << 3);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // ファンアウトリストの要素数
  // その他以下の情報もパックして持つ．
  // - 0      : 出力のマーク
  // - 1      : FFRの根のマーク
  // - 2      : EventQ に入っているかどうかを示すマーク
  // - 3      : 反転フラグ
  // - 4 - 15 : 最初のファンアウトの入力位置(FFR内のノードのみ意味を持つ)
  // - 16 -   : ファンアウト数
  unsigned int mFanoutNum;

  // ファンアウトの先頭のノード
  SimNode* mFanoutTop;

  // レベル
  SizeType mLevel;

  // イベントキューの次の要素
  SimNode* mLink;

  // 出力値
  FSIM_VALTYPE mVal;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIM_SIMNODE_H
