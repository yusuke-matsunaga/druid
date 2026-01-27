#ifndef FSIM_SIMNODE_H
#define FSIM_SIMNODE_H

/// @file SimNode.h
/// @brief SimNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "types/TpgNode.h"
#include "types/PackedVal.h"
#include "types/PackedVal3.h"


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
protected:

  /// @brief コンストラクタ
  SimNode(
    SizeType id,   ///< [in] ノード番号
    SizeType level ///< [in] レベル
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
    PrimType type,                 ///< [in] ゲートの種類
    SizeType level,                ///< [in] レベル
    const std::vector<SizeType>& inputs ///< [in] ファンインのノードのリスト
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
  PrimType
  gate_type() const = 0;

  /// @brief ファンイン数を得る．
  virtual
  SizeType
  fanin_num() const = 0;

  /// @brief pos 番めのファンインのノード番号を得る．
  virtual
  SizeType
  fanin(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < fanin_num() )
  ) const = 0;

  /// @brief ファンアウト数を得る．
  SizeType
  fanout_num() const
  {
    return static_cast<SizeType>(mFanoutNum >> 8);
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
    return static_cast<SizeType>(mFanoutNum & 0xFFU);
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
    return mFlags.test(FFR_ROOT);
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
    return mFlags.test(OUTPUT);
  }

  /// @brief 出力番号を返す.
  SizeType
  output_id() const
  {
    return mFanoutNum;
  }

  /// @brief 内容をダンプする．
  virtual
  void
  dump(
    std::ostream& s ///< [in] 出力ストリーム
  ) const = 0;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造に関する情報の設定用関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力マークをつける．
  void
  set_output(
    SizeType output_id
  )
  {
    mFlags.set(OUTPUT);
    mFanoutNum = output_id;
  }

  /// @brief ファンアウトリストを作成する．
  void
  set_fanout_list(
    const std::vector<SimNode*>& fo_list,
    SizeType ipos
  );

  /// @brief FFR の根の印をつける．
  void
  set_ffr_root()
  {
    mFlags.set(FFR_ROOT);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションに関する情報の取得/設定
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値を計算する．
  FSIM_VALTYPE
  calc_val(
    std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const
  {
    return _calc_val(val_array);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーション用の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力値の計算を行う．
  /// @return 計算結果を返す．
  virtual
  FSIM_VALTYPE
  _calc_val(
    const std::vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  ) const = 0;

  /// @brief ゲートの入力から出力までの可観測性を計算する．
  virtual
  PackedVal
  _calc_gobs(
    const std::vector<FSIM_VALTYPE>& val_array, ///< [in] 値の配列
    SizeType ipos                          ///< [in] 入力番号
  ) const = 0;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ID 番号
  SizeType mId;

  // フラグのビット位置を表す定数
  static const int OUTPUT = 0;
  static const int FFR_ROOT = 1;
  static const int NFLAGS = 2;

  // 種々のフラグ
  std::bitset<NFLAGS> mFlags;

  // ファンアウトリストの要素数
  // - 0 - 7 : 最初のファンアウトの入力位置(FFR内のノードのみ意味を持つ)
  // - 8 -   : ファンアウト数
  std::uint32_t mFanoutNum;

  // ファンアウトの先頭のノード
  SimNode* mFanoutTop;

  // レベル
  SizeType mLevel;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIM_SIMNODE_H
