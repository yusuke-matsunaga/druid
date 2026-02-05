#ifndef TPGBASE_H
#define TPGBASE_H

/// @file TpgBase.h
/// @brief TpgBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


BEGIN_NAMESPACE_DRUID

class TpgNetwork;
class NetworkRep;
class NodeRep;
class GateRep;
class FFRRep;
class MFFCRep;
class FaultRep;

//////////////////////////////////////////////////////////////////////
/// @class TpgBase TpgBase.h "TpgBase.h"
/// @brief NetworkRep の共有ポインタを持つ基底クラス
/// @ingroup TypesGroup
/// @sa TpgNode
///
/// TpgNetwork, TpgNode, TpgFFR, TpgMFFC, TpgFault などはこのクラス
/// を継承する．
//////////////////////////////////////////////////////////////////////
class TpgBase
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief 空のコンストラクタ
  ///
  /// 不正な値となる．
  TpgBase() = default;

  /// @brief 値を指定したコンストラクタ
  TpgBase(
    const std::shared_ptr<NetworkRep>& impl
  ) : mImpl{impl}
  {
  }

  /// @brief デストラクタ
  ~TpgBase() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 適正な値を持っているとき true を返す．
  bool
  is_valid() const
  {
    return mImpl.get() != nullptr;
  }

  /// @brief TpgNetworkを取り出す．
  TpgNetwork
  network() const;

  /// @brief 等価比較演算子
  bool
  operator==(
    const TpgBase& right
  ) const
  {
    return mImpl == right.mImpl;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const TpgBase& right
  ) const
  {
    return !operator==(right);
  }

  /// @brief 同じネットワークに属したオブジェクトの時に true を返す．
  static
  bool
  check_eq(
    const TpgBase& right,
    const TpgBase& left
  )
  {
    return right == left;
  }


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief NetworkRep の共有ポインタを取り出す．
  const std::shared_ptr<NetworkRep>&
  _network() const
  {
    return mImpl;
  }

  /// @brief ノード番号を TpgNode に変換する．
  TpgNode
  node(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief ノード番号のリストを TpgNodeList に変換する．
  TpgNodeList
  node_list(
    const std::vector<SizeType>& node_id_list
  ) const;

  /// @brief NodeRep* を TpgNode に変換する．
  TpgNode
  node(
    const NodeRep* node_rep
  ) const;

  /// @brief NodeRep* のリストを TpgNodeList に変換する．
  TpgNodeList
  node_list(
    const std::vector<const NodeRep*>& node_rep_list
  ) const;

  /// @brief ゲート番号を TpgGate に変換する．
  TpgGate
  gate(
    SizeType gate_id ///< [in] ゲート番号
  ) const;

  /// @brief ゲート番号のリストを TpgGateList に変換する．
  TpgGateList
  gate_list(
    const std::vector<SizeType>& gate_id_list ///< [in] ゲート番号のリスト
  ) const;

  /// @brief GateRep* を TpgGate に変換する．
  TpgGate
  gate(
    const GateRep* gate_rep
  ) const;

  /// @brief GateRep* のリストを TpgGateList に変換する．
  TpgGateList
  gate_list(
    const std::vector<const GateRep*>& gate_rep_list
  ) const;

  /// @brief FFR番号を TpgFFR に変換する．
  TpgFFR
  ffr(
    SizeType ffr_id ///< [in] FFR番号
  ) const;

  /// @brief FFR番号のリストを TpgFFRList に変換する．
  TpgFFRList
  ffr_list(
    const std::vector<SizeType>& ffr_id_list ///< [in] FFR番号のリスト
  ) const;

  /// @brief FFRRep* を TpgFFR に変換する．
  TpgFFR
  ffr(
    const FFRRep* ffr_rep
  ) const;

  /// @brief FFRRep* のリストを TpgFFRList に変換する．
  TpgFFRList
  ffr_list(
    const std::vector<const FFRRep*>& ffr_rep_list
  ) const;

  /// @brief MFFC番号を TpgMFFC に変換する．
  TpgMFFC
  mffc(
    SizeType mffc_id ///< [in] MFFC番号
  ) const;

  /// @brief MFFC番号のリストを TpgMFFCList に変換する．
  TpgMFFCList
  mffc_list(
    const std::vector<SizeType>& mffc_id_list ///< [in] MFFC番号のリスト
  ) const;

  /// @brief MFFCRep* を TpgMFFC に変換する．
  TpgMFFC
  mffc(
    const MFFCRep* mffc_rep
  ) const;

  /// @brief MFFCRep* のリストを TpgMFFCList に変換する．
  TpgMFFCList
  mffc_list(
    const std::vector<const MFFCRep*>& mffc_rep_list
  ) const;

  /// @brief 故障番号を TpgFault に変換する．
  TpgFault
  fault(
    SizeType fault_id ///< [in] 故障番号
  ) const;

  /// @brief 故障番号のリストを TpgFaultList に変換する．
  TpgFaultList
  fault_list(
    const std::vector<SizeType>& fault_id_list ///< [in] 故障番号
  ) const;

  /// @brief FaultRep* を TpgFault に変換する．
  TpgFault
  fault(
    const FaultRep* fault_rep
  ) const;

  /// @brief FaultRep* のリストを TpgFaultList に変換する．
  TpgFaultList
  fault_list(
    const std::vector<const FaultRep*>& fault_rep_list
  ) const;

  /// @brief NodeRep のポインタを取り出す．
  const NodeRep*
  _node(
    SizeType node_id ///< [in] ノード番号
  ) const;

  /// @brief NodeRep のポインタを取り出す．
  ///
  /// node が不正な値の場合は nullptr を返す．
  const NodeRep*
  _node(
    const TpgNode& node ///< [in] ノード
  ) const;

  /// @brief NodeRep のポインタのリストに変換する．
  std::vector<const NodeRep*>
  _node_list(
    const std::vector<SizeType>& id_list
  ) const;

  /// @brief GateRep のポインタを取り出す．
  const GateRep*
  _gate(
    SizeType gid ///< [in] ゲート番号
  ) const;

  /// @brief FFRRep のポインタを取り出す．
  const FFRRep*
  _ffr(
    SizeType ffr_id ///< [in] FFR番号
  ) const;

  /// @brief MFFCRep のポインタを取り出す．
  const MFFCRep*
  _mffc(
    SizeType mffc_id ///< [in] MFFC番号
  ) const;

  /// @brief FaultRep のポインタを取り出す．
  const FaultRep*
  _fault(
    SizeType fid ///< [in] 故障番号
  ) const;

  /// @brief mImpl が適正な値を持っているかチェックする．
  void
  _check_valid() const
  {
    if ( !is_valid() ) {
      abort();
      throw std::logic_error{"mImpl is null"};
    }
  }

  /// @brief 0 - (num - 1) のリストを作る．
  static
  std::vector<SizeType>
  make_id_list(
    SizeType num
  )
  {
    std::vector<SizeType> id_list(num);
    for ( SizeType id = 0; id < num; ++ id ) {
      id_list[id] = id;
    }
    return id_list;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgNetwork の実装
  std::shared_ptr<NetworkRep> mImpl;

};

END_NAMESPACE_DRUID

#endif // TPGBASE_H
