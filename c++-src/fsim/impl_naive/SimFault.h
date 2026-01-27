#ifndef SIMFAULT_H
#define SIMFAULT_H

/// @file SimFault.h
/// @brief SimFault のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "fsim_nsdef.h"
#include "types/TpgFault.h"


BEGIN_NAMESPACE_DRUID_FSIM

class SimNode;

//////////////////////////////////////////////////////////////////////
/// @class SimFault SimFault.h "SimFault.h"
/// @brief 故障シミュレーション用の故障関係のデータ構造
//////////////////////////////////////////////////////////////////////
class SimFault
{
public:

  /// @brief コンストラクタ
  SimFault(
    TpgFault f,                    ///< [in] オリジナルの故障
    SimNode* node,                 ///< [in] 故障伝搬の起点となるノード
    const std::vector<SimNode*>& simmap ///< [in] TpgNode と SimNode の対応関係
  );

  /// @brief デストラクタ
  ~SimFault() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障番号を返す．
  SizeType
  id() const
  {
    return mId;
  }

  /// @brief 故障伝搬の起点となるノードを返す．
  SimNode*
  origin_node() const
  {
    return mNode;
  }

  /// @brief 故障の活性化条件を求める．
  PackedVal
  excitation_condition() const;

#if FSIM_BSIDE
  /// @brief 遷移故障用の1時刻前の条件を求める．
  PackedVal
  previous_condition() const;
#endif

  /// @brief 故障伝搬マスクを得る．
  PackedVal
  obs_mask() const
  {
    return mObsMask;
  }

  /// @brief 故障伝搬マスクを設定する．
  void
  set_obs_mask(
    PackedVal val ///< [in] 設定する値
  )
  {
    mObsMask = val;
  }

  /// @brief スキップフラグを得る．
  bool
  skip() const
  {
    return mSkip;
  }

  /// @brief スキップフラグを設定する．
  void
  set_skip(
    bool flag ///< [in] 設定する値
  )
  {
    mSkip = flag;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  /// @brief ノードと値をパックした構造体
  class NodeVal
  {
  public:

    NodeVal() = default;

    NodeVal(
      SimNode* node,
      bool val
    ) : mBody{reinterpret_cast<PtrIntType>(node) | static_cast<PtrIntType>(val)}
    {
    }

    ~NodeVal() = default;


  public:
    //////////////////////////////////////////////////////////////////////
    // 外部インターフェイス
    //////////////////////////////////////////////////////////////////////

    /// @brief ノードを取り出す．
    SimNode*
    node() const
    {
      return reinterpret_cast<SimNode*>(mBody & ~1UL);
    }

    /// @brief 値を取り出す．
    bool
    val() const
    {
      return static_cast<bool>(mBody & 1);
    }


  private:
    //////////////////////////////////////////////////////////////////////
    // データメンバ
    //////////////////////////////////////////////////////////////////////

    // ノードと値をパックしたもの
    PtrIntType mBody{0};

  };

  // 故障番号
  SizeType mId;

  // 故障伝搬の起点となるノード
  SimNode* mNode;

  // 故障の励起条件のリスト
  std::vector<NodeVal> mExCondList;

#if FSIM_BSIDE
  // 遷移故障用の1時刻前の値割り当て
  std::vector<NodeVal> mPrevCondList;
#endif

  // 現在計算中のローカルな故障伝搬マスク
  PackedVal mObsMask{0};

  // スキップフラグ
  bool mSkip{false};

};

END_NAMESPACE_DRUID_FSIM

#endif // SIMFAULT_H
