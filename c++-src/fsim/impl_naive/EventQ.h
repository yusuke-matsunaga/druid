#ifndef FSIM_EVENTQ_H
#define FSIM_EVENTQ_H

/// @file EventQ.h
/// @brief EventQ のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "DiffBitsArray.h"
#include "SimNode.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class EventQ EventQ.h "EventQ.h"
/// @brief 故障シミュレーション用のイベントキュー
///
/// キューに詰まれる要素は SimNode で，各々のノードはレベルを持つ．
/// このキューではレベルの小さい順に処理してゆく．同じレベルのノード
/// 間の順序は任意でよい．
//////////////////////////////////////////////////////////////////////
class EventQ
{
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

  /// @brief 初期イベントを追加する．
  void
  put_event(
    SimNode* node,    ///< [in] 対象のノード
    PackedVal valmask ///< [in] 反転マスク
  )
  {
    if ( node->gate_type() == PrimType::None ) {
      // 入力の場合，他のイベントの干渉は受けないので
      // 今計算してしまう．
      auto old_val = node->val();
      node->set_val(old_val ^ valmask);
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
  DiffBitsArray
  simulate();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンアウトのノードをキューに積む．
  void
  put_fanouts(
    SimNode* node ///< [in] 対象のノード
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
    SimNode* node ///< [in] 対象のノード
  )
  {
    if ( !node->in_queue() ) {
      node->set_queue();
      auto level = node->level();
      auto& w = mArray[level];
      node->mLink = w;
      w = node;
      if ( mNum == 0 || mCurLevel > level ) {
	mCurLevel = level;
      }
      ++ mNum;
    }
  }

  /// @brief キューから取り出す．
  /// @retval nullptr キューが空だった．
  SimNode*
  get()
  {
    if ( mNum > 0 ) {
      // mNum が正しければ mCurLevel がオーバーフローすることはない．
      for ( ; ; ++ mCurLevel ) {
	auto& w = mArray[mCurLevel];
	auto node = w;
	if ( node != nullptr ) {
	  node->clear_queue();
	  w = node->mLink;
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
    SimNode* node,       ///< [in] 対象のノード
    FSIM_VALTYPE old_val ///< [in] 元の値
  )
  {
    mClearArray.push_back({node, old_val});
  }

  /// @brief 反転フラグをセットする．
  void
  set_flip_mask(
    SimNode* node,      ///< [in] 対象のノード
    PackedVal flip_mask ///< [in] 反転マスク
  )
  {
    node->set_flip();
    mFlipMaskArray[node->id()] = flip_mask;
    mMaskList[mMaskPos] = node;
    ++ mMaskPos;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // 値を元に戻すための構造体
  struct RestoreInfo
  {
    // ノード
    SimNode* mNode;

    // 元の値
    FSIM_VALTYPE mVal;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // キューの先頭ノードの配列
  std::vector<SimNode*> mArray;

  // 現在のレベル．
  SizeType mCurLevel{0};

  // キューに入っているノード数
  SizeType mNum{0};

  // clear 用の情報の配列
  std::vector<RestoreInfo> mClearArray;

  // mCelarArray の最後の要素位置
  SizeType mClearPos{0};

  // 反転マスクの配列
  // サイズは mClearArraySize と同じ
  std::vector<PackedVal> mFlipMaskArray;

  // 反転マスクをセットしたノードのリスト
  // 仕様上 PV_BITLEN が最大
  SimNode* mMaskList[PV_BITLEN];

  // mMaskList の最後の要素位置
  SizeType mMaskPos{0};

};

END_NAMESPACE_DRUID_FSIM

#endif // FSIM_EVENTQ_H
