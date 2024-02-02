#ifndef FSIM_EVENTQ_H
#define FSIM_EVENTQ_H

/// @file EventQ.h
/// @brief EventQ のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2022, 2024 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "SimNode.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class EventQ EventQ.h "EventQ.h"
/// @brief 故障シミュレーション用のイベントキュー
///
/// キューに詰まれる要素は EvNode で，各々のノードはレベルを持つ．
/// このキューではレベルの小さい順に処理してゆく．同じレベルのノード
/// 間の順序は任意でよい．
//////////////////////////////////////////////////////////////////////
class EventQ
{
private:

  /// @brief イベントを表す構造体
  struct Event
  {
    /// @brief 対象の SimNode
    const SimNode* mSimNode;

    /// @brief イベントキューの次の要素
    Event* mLink{nullptr};
  };


public:

  /// @brief コンストラクタ
  EventQ() = default;

  /// @brief デストラクタ
  ~EventQ() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  init(
    SizeType max_level,  ///< [in] 最大レベル
    SizeType output_num, ///< [in] 出力数
    SizeType node_num    ///< [in] ノード数
  );

  /// @brief 値をコピーする．
  void
  copy_val(
    const vector<FSIM_VALTYPE>& val_array ///< [in] 値の配列
  )
  {
    SizeType n = mValArray.size();
    ASSERT_COND( val_array.size() == n );
    for ( SizeType i = 0; i < n; ++ i ) {
      mValArray[i] = val_array[i];
    }
  }

  /// @brief 初期イベントを追加する．
  void
  put_event(
    const SimNode* node, ///< [in] 対象のノード
    PackedVal valmask    ///< [in] 反転マスク
  )
  {
    if ( node->gate_type() == PrimType::None ) {
      // 入力の場合，他のイベントの干渉は受けないので
      // 今計算してしまう．
      auto old_val = get_val(node);
      set_val(node, old_val ^ valmask);
      add_to_clear_list(node, old_val);
      put_fanouts(node);
    }
    else {
      // 複数のイベントを登録する場合があるので
      // ここでは計算せずに反転マスクのみをセットする．
      set_flip_mask(node, valmask);
      put(node);
    }
  }

  /// @brief イベントドリブンシミュレーションを行う．
  /// @retval 出力における変化ビットを返す．
  PackedVal
  simulate();

  /// @brief mPropArray をクリアする．
  void
  clear_prop_val();

  /// @brief 出力ごとの結果を得る．
  PackedVal
  prop_val(
    SizeType pos ///< [in] 出力番号
  )
  {
    return mPropArray[pos];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードの値を取り出す．
  FSIM_VALTYPE
  get_val(
    const SimNode* node ///< [in] 対象のノード
  )
  {
    return mValArray[node->id()];
  }

  /// @brief ノードの値を設定する．
  void
  set_val(
    const SimNode* node, ///< [in] 対象のノード
    FSIM_VALTYPE val     ///< [in] 設定する値
  )
  {
    mValArray[node->id()] = val;
  }

  /// @brief ファンアウトのノードをキューに積む．
  void
  put_fanouts(
    const SimNode* node ///< [in] 対象のノード
  )
  {
    auto no = node->fanout_num();
    if ( no == 1 ) {
      put(node->fanout_top());
    }
    else {
      for ( auto i: Range(0, no) ) {
	put(node->fanout(i));
      }
    }
  }

  /// @brief キューに積む
  void
  put(
    const SimNode* node ///< [in] 対象のノード
  )
  {
    if ( mEventMap[node->id()] ) {
      auto ev = new Event{node, nullptr};
      ++ mNum;
      mEventMap[node->id()] = ev;
      auto level = node->level();
      auto& w = mArray[level];
      ev->mLink = w;
      w = ev;
      if ( mNum == 1 || mCurLevel > level ) {
	mCurLevel = level;
      }
    }
  }

  /// @brief キューから取り出す．
  /// @retval nullptr キューが空だった．
  const SimNode*
  get()
  {
    if ( mNum > 0 ) {
      // mNum が正しければ mCurLevel がオーバーフローすることはない．
      for ( ; ; ++ mCurLevel ) {
	auto& w = mArray[mCurLevel];
	auto ev = w;
	if ( ev != nullptr ) {
	  auto node = ev->mSimNode;
	  mEventMap[node->id()] = nullptr;
	  w = ev->mLink;
	  delete ev;
	  -- mNum;
	  return node;
	}
      }
    }
    return nullptr;
  }

  /// @brief clear リストに追加する．
  void
  add_to_clear_list(
    const SimNode* node, ///< [in] 対象のノード
    FSIM_VALTYPE old_val ///< [in] 元の値
  )
  {
    mClearArray.push_back({node->id(), old_val});
  }

  /// @brief 反転フラグをセットする．
  void
  set_flip_mask(
    const SimNode* node, ///< [in] 対象のノード
    PackedVal flip_mask  ///< [in] 反転マスク
  )
  {
    ASSERT_COND( mFlipMaskArray[node->id()] == PV_ALL0 );
    mFlipMaskArray[node->id()] = flip_mask;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 値を元に戻すための構造体
  struct RestoreInfo
  {
    // ノード番号
    SizeType mId;

    // 元の値
    FSIM_VALTYPE mVal;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力ごとの故障伝搬パタンの配列
  vector<PackedVal> mPropArray;

  // レベルごとのキューの先頭ノードの配列
  vector<Event*> mArray;

  // 現在のレベル．
  SizeType mCurLevel{0};

  // キューに入っているノード数
  SizeType mNum{0};

  // ノード番号をキーにして Event を保持する配列
  // キューに入っていない場合には nullptr を持つ．
  vector<Event*> mEventMap;

  // ノード番号をキーにして反転マスクを保持する配列
  vector<PackedVal> mFlipMaskArray;

  // 値の配列
  vector<FSIM_VALTYPE> mValArray;

  // clear 用の情報の配列
  vector<RestoreInfo> mClearArray;

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIM_EVENTQ_H
