#ifndef FSIM_H
#define FSIM_H

/// @file Fsim.h
/// @brief Fsim のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "types/FaultType.h"
#include "types/PackedVal.h"
#include "fsim/DiffBits.h"
#include "fsim/DiffBitsArray.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_DRUID

class FsimImpl;

//////////////////////////////////////////////////////////////////////
/// @class Fsim Fsim.h "Fsim.h"
/// @brief 故障シミュレーションを行うクラス
/// @ingroup FsimGroup
/// @sa DiffBits, DiffBitsArray
///
/// 意味的には対象回路に対して故障シミュレーションを行う関数のみを実現できればよく，
/// 内部に状態を持つ必要はないが，効率化のため故障シミュレーションに特化した回路構造
/// を表すクラスと故障シミュレーションに特化した故障の情報を表すクラスを持つ．
/// 対象の回路を initialize() で設定すると内部で故障シミュレーション用のデータ構造
/// に変換される．
/// 対象となる故障も毎回のシミュレーションの際に指定すると効率が悪いので一旦全故障を
/// set_fault_list() で登録しておく．
/// 各故障に'スキップフラグ'を持たせておき，スキップフラグが立っていない故障を対象に
/// 故障シミュレーションを行う．
/// スキップフラグのオン/オフは set_skip()/clear_skip() で行う．
//////////////////////////////////////////////////////////////////////
class Fsim
{
public:

  /// @brief コンストラクタ
  Fsim(
    const TpgNetwork& network,      ///< [in] ネットワーク
    const TpgFaultList& fault_list, ///< [in] 対象の故障のリスト
    const JsonValue& option = {}    ///< [in] オプション
  );

  /// @brief コピーコンストラクタは禁止
  Fsim(const Fsim& src) = delete;

  /// @brief ムーブコンストラクタは禁止
  Fsim(Fsim&& src) = delete;

  /// @brief コピー代入演算子は禁止
  Fsim&
  operator=(const Fsim& src) = delete;

  /// @brief ムーブ代入演算子は禁止
  Fsim&
  operator=(Fsim&& src) = delete;

  /// @brief デストラクタ
  ~Fsim();


public:
  //////////////////////////////////////////////////////////////////////
  // 故障を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障にスキップマークをつける．
  void
  set_skip_all();

  /// @brief 故障にスキップマークをつける．
  void
  set_skip(
    const TpgFault& fault ///< [in] 対象の故障
  );

  /// @brief 複数の故障にスキップマークをつける．
  ///
  /// fault_list に含まれない故障のスキップマークは消される．
  void
  set_skip(
    const TpgFaultList& fault_list ///< [in] 対象の故障のリスト
  );

  /// @brief 全ての故障のスキップマークを消す．
  void
  clear_skip_all();

  /// @brief 故障のスキップマークを消す．
  void
  clear_skip(
    const TpgFault& fault ///< [in] 対象の故障番号
  );

  /// @brief 複数の故障のスキップマークを消す．
  ///
  /// fault_list に含まれない故障のスキップマークは付けられる．
  void
  clear_skip(
    const TpgFaultList& fault_list ///< [in] 対象の故障のリスト
  );

  /// @brief 故障のスキップマークを得る．
  bool
  get_skip(
    const TpgFault& fault ///< [in] 対象の故障番号
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPPFP故障シミュレーションで用いるコールバック関数の型定義
  ///
  /// * 1番目の引数は検出された故障
  /// * 2番目の引数は出力ごとの伝搬状況
  using cbtype1 = std::function<void(const TpgFault&, const DiffBits&)>;

  /// @brief PPSFP故障シミュレーションで用いるコールバック関数の型定義
  ///
  /// * 1番目の引数は検出された故障
  /// * 2番目の引数は出力ごとの伝搬状況
  using cbtype2 = std::function<void(const TpgFault&, const DiffBitsArray&)>;

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  spsfp(
    const TestVector& tv,  ///< [in] テストベクタ
    const TpgFault& fault, ///< [in] 対象の故障番号
    DiffBits& dbits        ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  );

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  ///
  /// assign_list は外部入力の割り当てでなければならない．
  bool
  spsfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    const TpgFault& fault,         ///< [in] 対象の故障番号
    DiffBits& dbits                ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  );

  /// @brief SPSFP故障シミュレーションを行う．
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  bool
  xspsfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    const TpgFault& fault,         ///< [in] 対象の故障番号
    DiffBits& dbits                ///< [out] 出力ごとの伝搬状況を表すビットベクタ
  );

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  void
  sppfp(
    const TestVector& tv,  ///< [in] テストベクタ
    cbtype1 callback       ///< [in] コールバック関数
                           ///<      1番目の引数は検出された故障
                           ///<      2番目の引数は出力の伝搬状況
  );

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  ///
  /// assign_list は外部入力の割り当てでなければならない．
  void
  sppfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    cbtype1 callback               ///< [in] コールバック関数
                                   ///<      1番目の引数は検出された故障
                                   ///<      2番目の引数は出力の伝搬状況
  );

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  ///
  /// * assign_list は任意の位置の割り当てでよい．
  /// * 3値のシミュレーションのみ可能
  void
  xsppfp(
    const AssignList& assign_list, ///< [in] 値の割当リスト
    cbtype1 callback               ///< [in] コールバック関数
                                   ///<      1番目の引数は検出された故障
                                   ///<      2番目の引数は出力の伝搬状況
  );

  /// @brief 複数のパタンで故障シミュレーションを行う．
  void
  ppsfp(
    const std::vector<TestVector>& tv_list, ///< [in] テストベクタのリスト
                                            ///<      要素数の最大値は PV_BITLEN
    cbtype2 callback                        ///< [in] コールバック関数
                                            ///<      1番目の引数は検出された故障
                                            ///<      2番目の引数は出力の伝搬状況
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 順序回路用のシミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  ///
  /// - 外部入力以外は無視する．
  /// - 時刻1の割り当ても無視する
  SizeType
  calc_wsa(
    const TestVector& tv,  ///< [in] テストベクタ
    bool weighted          ///< [in] 重みフラグ
                           ///< - false: ゲートの出力の遷移回数の和
                           ///< - true : ゲートの出力の遷移回数に
                           ///<          (ファンアウト数＋１)を掛けたものの和
  );

  /// @brief 1クロック分のシミュレーションを行い，遷移回数を数える．
  SizeType
  calc_wsa(
    const InputVector& i_vect,  ///< [in] 外部入力のビットベクタ
    bool weighted               ///< [in] 重みフラグ
				///< - false: ゲートの出力の遷移回数の和
				///< - true : ゲートの出力の遷移回数に
				///<          (ファンアウト数＋１)を掛けたものの和
  );

  /// @brief 状態を設定する．
  void
  set_state(
    const InputVector& i_vect, ///< [in] 外部入力のビットベクタ
    const DffVector& f_vect    ///< [in] FFの値のビットベクタ
  );

  /// @brief 状態を取得する．
  void
  get_state(
    InputVector& i_vect, ///< [out] 外部入力のビットベクタ
    DffVector& f_vect	 ///< [out] FFの値のビットベクタ
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 定数
  //////////////////////////////////////////////////////////////////////

  /// @brief ppsfp() で用いるバッファサイズ
  static
  const SizeType
  PP_BITLEN = PV_BITLEN;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  initialize_multi(
    const TpgNetwork& network,      ///< [in] ネットワーク
    const TpgFaultList& fault_list, ///< [in] 対象の故障のリスト
    bool has_previous_state,        ///< [in] 1時刻前の値を持つ時 true にする．
    bool has_x                      ///< [in] 3値のシミュレーションを行う時 true にする．
  );

  /// @brief 初期化を行う．
  void
  initialize_naive(
    const TpgNetwork& network,      ///< [in] ネットワーク
    const TpgFaultList& fault_list, ///< [in] 対象の故障のリスト
    bool has_previous_state,        ///< [in] 1時刻前の値を持つ時 true にする．
    bool has_x                      ///< [in] 3値のシミュレーションを行う時 true にする．
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象のネットワーク
  const TpgNetwork& mNetwork;

  // 実装クラス
  std::unique_ptr<FsimImpl> mImpl;

};

END_NAMESPACE_DRUID

#endif // FSIM_H
