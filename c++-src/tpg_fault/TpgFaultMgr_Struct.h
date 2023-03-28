#ifndef TPGFAULTMGR_STRUCT_H
#define TPGFAULTMGR_STRUCT_H

/// @file TpgFaultMgr_Struct.h
/// @brief TpgFaultMgr_Struct のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "TpgFaultMgrImpl.h"


BEGIN_NAMESPACE_DRUID

class TpgGate;
class TpgFaultImpl;

//////////////////////////////////////////////////////////////////////
/// @class TpgFaultMgr_Struct TpgFaultMgr_Struct.h "TpgFaultMgr_Struct.h"
/// @brief StuckAt/TransitionDelay 故障を作るためのクラス
//////////////////////////////////////////////////////////////////////
class TpgFaultMgr_Struct :
  public TpgFaultMgrImpl
{
public:

  /// @brief コンストラクタ
  TpgFaultMgr_Struct(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) : TpgFaultMgrImpl{network}
  {
  }

  /// @brief デストラクタ
  ~TpgFaultMgr_Struct() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 全ての故障を作る．
  void
  _gen_all_faults(
    const TpgNetwork& network,       ///< [in] 対象のネットワーク
    unordered_set<SizeType>& rep_map ///< [in] 代表故障番号を表す辞書
  ) override;

  /// @brief 出力の故障を作る．
  ///
  /// 結果は mNodeInfoArray に格納する．
  void
  gen_ofault(
    const TpgNode* node,             ///< [in] ノード
    const string& node_name,         ///< [in] ノード名
    unordered_set<SizeType>& rep_map ///< [in] 代表故障番号を表す辞書
  );

  /// @brief 入力の故障を作る．
  ///
  /// 結果は mNodeInfoArray に格納する．
  void
  gen_ifault(
    const TpgGate& gate,             ///< [in] ゲート情報
    const TpgNode* node,             ///< [in] ノード
    const string& node_name,         ///< [in] ノード名
    SizeType ipos,                   ///< [in] 入力位置
    unordered_set<SizeType>& rep_map ///< [in] 代表故障番号を表す辞書
  );

  /// @brief 入力の故障を作る(PPO用)．
  ///
  /// 結果は mNodeInfoArray に格納する．
  void
  gen_ifault(
    const TpgNode* node,             ///< [in] ノード
    const string& node_name,         ///< [in] ノード名
    unordered_set<SizeType>& rep_map ///< [in] 代表故障番号を表す辞書
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力の故障を作る．
  virtual
  TpgFaultImpl*
  new_ofault(
    const TpgNode* node,     ///< [in] ノード
    const string& node_name, ///< [in] ノード名
    Fval2 fval               ///< [in] 故障値
  ) = 0;

  /// @brief 入力の故障を作る．
  virtual
  TpgFaultImpl*
  new_ifault(
    const TpgNode* node,     ///< [in] ノード
    const string& node_name, ///< [in] ノード名
    SizeType ipos,           ///< [in] 入力位置
    Fval2 fval               ///< [in] 故障値
  ) = 0;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFaultMgr_Sa TpgFaultMgr_Struct.h "TpgFaultMgr_Struct.h"
/// @brief StuckAt Fault 用の FaulgMgr
//////////////////////////////////////////////////////////////////////
class TpgFaultMgr_Sa :
  public TpgFaultMgr_Struct
{
public:

  /// @brief コンストラクタ
  TpgFaultMgr_Sa(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) : TpgFaultMgr_Struct{network}
  {
  }

  /// @brief デストラクタ
  ~TpgFaultMgr_Sa() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のタイプを返す．
  FaultType
  fault_type() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力の故障を作る．
  TpgFaultImpl*
  new_ofault(
    const TpgNode* node,     ///< [in] ノード
    const string& node_name, ///< [in] ノード名
    Fval2 fval               ///< [in] 故障値
  ) override;

  /// @brief 入力の故障を作る．
  TpgFaultImpl*
  new_ifault(
    const TpgNode* node,     ///< [in] ノード
    const string& node_name, ///< [in] ノード名
    SizeType ipos,           ///< [in] 入力位置
    Fval2 fval               ///< [in] 故障値
  ) override;

};


//////////////////////////////////////////////////////////////////////
/// @class TpgFaultMgr_Td TpgFaultMgr_Struct.h "TpgFaultMgr_Struct.h"
/// @brief TransitionDelay Fault 用の TpgFaultMgr
//////////////////////////////////////////////////////////////////////
class TpgFaultMgr_Td :
  public TpgFaultMgr_Struct
{
public:

  /// @brief コンストラクタ
  TpgFaultMgr_Td(
    const TpgNetwork& network ///< [in] 対象のネットワーク
  ) : TpgFaultMgr_Struct{network}
  {
  }

  /// @brief デストラクタ
  ~TpgFaultMgr_Td() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障のタイプを返す．
  FaultType
  fault_type() const override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 出力の故障を作る．
  TpgFaultImpl*
  new_ofault(
    const TpgNode* node,     ///< [in] ノード
    const string& node_name, ///< [in] ノード名
    Fval2 fval               ///< [in] 故障値
  ) override;

  /// @brief 入力の故障を作る．
  TpgFaultImpl*
  new_ifault(
    const TpgNode* node,     ///< [in] ノード
    const string& node_name, ///< [in] ノード名
    SizeType ipos,           ///< [in] 入力位置
    Fval2 fval               ///< [in] 故障値
  ) override;

};

END_NAMESPACE_DRUID

#endif // TPGFAULTMGR_STRUCT_H
