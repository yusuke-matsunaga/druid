#ifndef TPGNETWORKIMPL_H
#define TPGNETWORKIMPL_H

/// @file TpgNetworkImpl.h
/// @brief TpgNetworkImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/logic.h"
#include "ym/Array.h"

#include "TpgDff.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"

BEGIN_NAMESPACE_DRUID

class TpgNodeInfo;
class AuxNodeInfo;
class TpgGateInfo;
class TpgFaultBase;

//////////////////////////////////////////////////////////////////////
/// @class TpgNetworkImpl TpgNetworkImpl.h "TpgNetworkImpl.h"
/// @brief TpgNetwork の実装クラス
//////////////////////////////////////////////////////////////////////
class TpgNetworkImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  TpgNetworkImpl();

  /// @brief デストラクタ
  ~TpgNetworkImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 通常の構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ノード数を得る．
  int
  node_num() const
  {
    return mNodeArray.size();
  }

  /// @brief ノードを得る．
  ///
  /// @code
  /// node = network.node(node->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  node(
    int id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const
  {
    ASSERT_COND( id >= 0 && id < node_num() );

    return mNodeArray[id];
  }

  /// @brief 全ノードのリストを得る．
  const vector<const TpgNode*>&
  node_list() const
  {
    return mNodeArray;
  }

  /// @brief ノード名を得る．
  const string&
  node_name(
    int id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const;

  /// @brief 外部入力数を得る．
  int
  input_num() const
  {
    return mInputNum;
  }

  /// @brief 外部入力ノードを得る．
  ///
  /// @code
  /// node = network.input(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  input(
    int pos ///< [in] 位置番号 ( 0 <= pos < input_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < input_num() );

    return mPPIArray[pos];
  }

  /// @brief 外部入力ノードのリストを得る．
  const vector<const TpgNode*>&
  input_list() const
  {
    return mPPIArray;
  }

  /// @brief 外部出力数を得る．
  int
  output_num() const
  {
    return mOutputNum;
  }

  /// @brief 外部出力ノードを得る．
  ///
  /// @code
  /// node = network.output(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output(
    int pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < output_num() );

    return mPPOArray[pos];
  }

  /// @brief 外部出力ノードのリストを得る．
  const vector<const TpgNode*>&
  output_list() const
  {
    return mPPOArray;
  }

  /// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
  ///
  /// @code
  /// node = network.output2(node->output_id2())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output2(
    int pos ///< [in] 位置番号 ( 0 <= pos < output_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < output_num() );

    return mPPOArray2[pos];
  }

  /// @brief スキャン方式の擬似外部入力数を得る．
  ///
  /// = input_num() + dff_num()
  int
  ppi_num() const
  {
    return input_num() + dff_num();
  }

  /// @brief スキャン方式の擬似外部入力を得る．
  ///
  /// @code
  /// node = network.ppi(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppi(
    int pos ///< [in] 位置番号 ( 0 <= pos < ppi_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < ppi_num() );

    return mPPIArray[pos];
  }

  /// @brief 擬似外部入力のリストを得る．
  const vector<const TpgNode*>&
  ppi_list() const
  {
    return mPPIArray;
  }

  /// @brief スキャン方式の擬似外部出力数を得る．
  ///
  /// = output_num() + dff_num()
  int
  ppo_num() const
  {
    return output_num() + dff_num();
  }

  /// @brief スキャン方式の擬似外部出力を得る．
  ///
  /// @code
  /// node = network.ppo(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppo(
    int pos ///< [in] 位置番号 ( 0 <= pos < ppo_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < ppo_num() );

    return mPPOArray[pos];
  }

  /// @brief 擬似外部出力のリストを得る．
  const vector<const TpgNode*>&
  ppo_list() const
  {
    return mPPOArray;
  }

  /// @brief MFFC 数を返す．
  int
  mffc_num() const
  {
    return mMffcArray.size();
  }

  /// @brief MFFC を返す．
  const TpgMFFC&
  mffc(
    int pos ///< [in] 位置番号 ( 0 <= pos < mffc_num() )
  ) const;

  /// @brief MFFC のリストを得る．
  const vector<TpgMFFC>&
  mffc_list() const;

  /// @brief FFR 数を返す．
  int
  ffr_num() const
  {
    return mFfrArray.size();
  }

  /// @brief FFR を返す．
  const TpgFFR&
  ffr(
    int pos ///< [in] 位置番号 ( 0 <= pos < ffr_num() )
  ) const;

  /// @brief FFR のリストを得る．
  const vector<TpgFFR>&
  ffr_list() const;

  /// @brief DFF数を得る．
  int
  dff_num() const
  {
    return mDffArray.size();
  }

  /// @brief DFF を得る．
  ///
  /// @code
  /// dff = network.dff(dff->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgDff&
  dff(
    int pos ///< [in] 位置番号 ( 0 <= pos < dff_num() )
  ) const;

  /// @brief DFF のリストを得る．
  const vector<TpgDff>&
  dff_list() const;

  /// @brief 故障IDの最大値+1を返す．
  int
  max_fault_id() const
  {
    return mFaultNum;
  }

  /// @brief 全代表故障数を返す．
  int
  rep_fault_num() const
  {
    return mRepFaultArray.size();
  }

  /// @brief 代表故障を返す．
  const TpgFault*
  rep_fault(
    int pos ///< [in] 位置番号 ( 0 <= pos < rep_fault_num() )
  ) const
  {
    ASSERT_COND( pos >= 0 && pos < rep_fault_num() );

    return mRepFaultArray[pos];
  }

  /// @brief 代表故障のリストを返す．
  const vector<const TpgFault*>&
  rep_fault_list() const
  {
    return mRepFaultArray;
  }

  /// @brief ノードに関係した代表故障数を返す．
  int
  node_rep_fault_num(
    int id ///< [in] ID番号 ( 0 <= id < node_num() )
  ) const;

  /// @brief ノードに関係した代表故障を返す．
  const TpgFault*
  node_rep_fault(
    int id,  ///< [in] ID番号 ( 0 <= id < node_num() )
    int pos  ///< [in] 位置番号 ( 0 <= pos < node_rep_fault_num(id) )
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定するための関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork から内容を設定する．
  void
  set(
    const BnNetwork& network ///< [in] 設定元のネットワーク
  );

  /// @brief サイズを設定する．
  void
  set_size(
    int input_num,
    int output_num,
    int dff_num,
    int node_num
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 複合ゲートの入力の情報
  ///
  /// もとの入力が実際の TpgNode のどのファンインに
  /// 対応しているかを表す．
  struct InodeInfo
  {
    InodeInfo(
      const TpgNode* node = nullptr,
      SizeType pos = 0
    ) : mNode{node},
	mPos{pos}
    {
    }

    void
    set(
      const TpgNode* node,
      int pos
    )
    {
      mNode = node;
      mPos = pos;
    }

    const TpgNode* mNode;
    SizeType mPos;
  };


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief 入力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_input_node(
    int iid,            ///< [in] 入力の番号
    const string& name, ///< [in] ノード名
    SizeType fanout_num ///< [in] ファンアウト数
  );

  /// @brief 出力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_output_node(
    int oid,             ///< [in] 出力の番号
    const string& name,  ///< [in] ノード名
    const TpgNode* inode ///< [in] 入力のノード
  );

  /// @brief DFFの入力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_input_node(
    int oid,             ///< [in] 出力の番号
    const TpgDff* dff,   ///< [in] 接続しているDFF
    const string& name,  ///< [in] ノード名
    const TpgNode* inode ///< [in] 入力のノード
  );

  /// @brief DFFの出力ノードを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_output_node(
    int iid,             ///< [in] 入力の番号
    const TpgDff* dff,   ///< [in] 接続しているDFF
    const string& name,  ///< [in] ノード名
    SizeType fanout_num  ///< [in] ファンアウト数
  );

  /// @brief DFFのクロック端子を生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_clock_node(
    const TpgDff* dff,   ///< [in] 接続しているDFF
    const string& name,  ///< [in] ノード名
    const TpgNode* inode ///< [in] 入力のノード
  );

  /// @brief DFFのクリア端子を生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_clear_node(
    const TpgDff* dff,   ///< [in] 接続しているDFF
    const string& name,  ///< [in] ノード名
    const TpgNode* inode ///< [in] 入力のノード
  );

  /// @brief DFFのプリセット端子を生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_preset_node(
    const TpgDff* dff,   ///< [in] 接続しているDFF
    const string& name,  ///< [in] ノード名
    const TpgNode* inode ///< [in] 入力のノード
  );

  /// @brief 論理ノードを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_logic_node(
    const string& name,                       ///< [in] ノード名
    const TpgGateInfo* node_info,             ///< [in] 論理関数の情報
    const vector<const TpgNode*>& fanin_list, ///< [in] ファンインのリスト
    SizeType fanout_num,                      ///< [in] ファンアウト数
    vector<pair<int, int>>& connection_list   ///< [in] 接続リスト
  );

  /// @brief 論理式から TpgNode の木を生成する．
  ///
  /// leaf_nodes は 変数番号 * 2 + (0/1) に
  /// 該当する変数の肯定/否定のリテラルが入っている．
  TpgNode*
  make_cplx_node(
    const string& name,                       ///< [in] ノード名
    const Expr& expr,			      ///< [in] 式
    const vector<const TpgNode*>& leaf_nodes, ///< [in] 式のリテラルに対応するノードの配列
    vector<InodeInfo>& inode_array,	      ///< [in] ファンインの対応関係を収める配列
    SizeType fanout_num,		      ///< [in] ファンアウト数
    vector<pair<int, int>>& connection_list   ///< [out] 接続リスト
  );

  /// @brief バッファを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_buff_node(
    const string& name,                     ///< [in] ノード名
    const TpgNode* fanin,		    ///< [in] ファンインのノード
    SizeType fanout_num,		    ///< [in] ファンアウト数
    vector<pair<int, int>>& connection_list ///< [out] 接続リスト
  );

  /// @brief インバーターを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_not_node(
    const string& name,                     ///< [in] ノード名
    const TpgNode* fanin,		    ///< [in] ファンインのノード
    SizeType fanout_num,		    ///< [in] ファンアウト数
    vector<pair<int, int>>& connection_list ///< [out] 接続リスト
  );

  /// @brief 組み込み型の論理ゲートを生成する．
  /// @return 生成したノードを返す．
  TpgNode*
  make_prim_node(
    const string& name,                       ///< [in] ノード名
    GateType type,			      ///< [in] ゲートの型
    const vector<const TpgNode*>& fanin_list, ///< [in] ファンインのリスト
    SizeType fanout_num,                      ///< [in] ファンアウト数
    vector<pair<int, int>>& connection_list   ///< [out] 接続リスト
  );

  /// @brief 論理ノードを作る．
  /// @return 作成したノードを返す．
  TpgNode*
  make_logic(
    GateType gate_type,                       ///< [in] ゲートタイプ
    const vector<const TpgNode*>& inode_list, ///< [in] 入力ノードのリスト
    SizeType fanout_num                       ///< [in] ファンアウト数
  );

  /// @brief make_XXX_node の共通処理
  void
  make_node_common(
    TpgNode* node,      ///< [in] ノード
    const string& name, ///< [in] ノード名
    SizeType ni         ///< [in] ファンイン数
  );

  /// @brief 出力の故障を作る．
  void
  new_ofault(
    const string& name, ///< [in] 故障位置のノード名
    int val,		///< [in] 故障値 ( 0 / 1 )
    const TpgNode* node	///< [in] 故障位置のノード
  );

  /// @brief 入力の故障を作る．
  ///
  /// プリミティブ型の場合は ipos と inode_pos は同一だが
  /// 複合型の場合には異なる．
  void
  new_ifault(
    const string& name,          ///< [in] 故障位置のノード名
    SizeType ipos,		 ///< [in] 故障位置のファンイン番号
    int val,			 ///< [in] 故障値 ( 0 / 1 )
    const InodeInfo& inode_info, ///< [in] TpgNode 上のノードの情報
    TpgFault* rep		 ///< [in] 代表故障
  );

  /// @brief 出力の故障を得る．
  TpgFaultBase*
  _node_output_fault(
    int id, ///< [in] ノードID ( 0 <= id < node_num() )
    int val ///< [in] 故障値 ( 0 / 1 )
  );

  /// @brief 入力の故障を得る．
  TpgFaultBase*
  _node_input_fault(
    int id,       ///< [in] id ノードID ( 0 <= id < node_num() )
    int val,      ///< [in] val 故障値 ( 0 / 1 )
    SizeType pos  ///< [in] pos 入力の位置番号
  );


  /// @brief 代表故障を設定する．
  int
  set_rep_faults(
    const TpgNode* node ///< [in] 対象のノード
  );

  /// @brief FFR の情報を設定する．
  void
  set_ffr(
    const TpgNode* root, ///< [in] FFR の根のノード
    TpgFFR* ffr		 ///< [in] 対象の FFR
  );

  /// @brief MFFC の情報を設定する．
  void
  set_mffc(
    const TpgNode* root, ///< [in] root MFFCの根のノード
    TpgMFFC* mffc	 ///< [in] mffc 対象のMFFC
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  int mInputNum{0};

  // 出力数
  int mOutputNum{0};

  // DFFの実体の配列
  vector<TpgDff> mDffArray;

  // ノードのポインタ配列
  vector<const TpgNode*> mNodeArray;

  // ノードの付加情報の配列
  vector<AuxNodeInfo> mAuxInfoArray;

  // PPIノードの配列
  vector<const TpgNode*> mPPIArray;

  // PPOノードの配列
  vector<const TpgNode*> mPPOArray;

  // TFI サイズの降順に整列したPPOノードの配列
  vector<const TpgNode*> mPPOArray2;

  // MFFC の本体の配列
  vector<TpgMFFC> mMffcArray;

  // FFR の本体の配列
  vector<TpgFFR> mFfrArray;

  // 全故障数
  int mFaultNum{0};

  // 代表故障のポインタ配列
  vector<const TpgFault*> mRepFaultArray;

};

#if 0
//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ノード数を得る．
inline
int
TpgNetworkImpl::node_num() const
{
  return mNodeNum;
}

// @brief ノードを得る．
inline
const TpgNode*
TpgNetworkImpl::node(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < mNodeNum );

  return mNodeArray[pos];
}

// @brief 全ノードのリストを得る．
inline
Array<const TpgNode*>
TpgNetworkImpl::node_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mNodeArray), 0, mNodeNum);
}

// @brief 外部入力数を得る．
inline
int
TpgNetworkImpl::input_num() const
{
  return mInputNum;
}

// @brief 外部入力ノードを得る．
//< [in] pos 位置番号 ( 0 <= pos < input_num() )
inline
const TpgNode*
TpgNetworkImpl::input(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < input_num() );

  return mPPIArray[pos];
}

// @brief 外部入力ノードのリストを得る．
inline
Array<const TpgNode*>
TpgNetworkImpl::input_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mPPIArray), 0, mInputNum);
}

// @brief 外部出力数を得る．
inline
int
TpgNetworkImpl::output_num() const
{
  return mOutputNum;
}

// @brief 外部出力ノードを得る．
//< [in] pos 位置番号 ( 0 <= pos < output_num() )
inline
const TpgNode*
TpgNetworkImpl::output(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < output_num() );

  return mPPIArray[pos];
}

// @brief 外部出力ノードのリストを得る．
inline
Array<const TpgNode*>
TpgNetworkImpl::output_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mPPOArray), 0, mOutputNum);
}

// @brief サイズの降順で整列した順番で外部出力ノードを取り出す．
inline
const TpgNode*
TpgNetworkImpl::output2(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < output_num() );

  return mPPOArray2[pos];
}

// @brief DFF数を得る．
inline
int
TpgNetworkImpl::dff_num() const
{
  return mDffNum;
}

// @brief スキャン方式の擬似外部入力数を得る．
//
// = input_num() + dff_num()
inline
int
TpgNetworkImpl::ppi_num() const
{
  return mInputNum + mDffNum;
}

// @brief スキャン方式の擬似外部入力を得る．
//< [in] pos 位置番号 ( 0 <= pos < ppi_num() )
inline
const TpgNode*
TpgNetworkImpl::ppi(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < ppi_num() );

  return mPPIArray[pos];
}

// @brief 擬似外部入力のリストを得る．
inline
Array<const TpgNode*>
TpgNetworkImpl::ppi_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mPPIArray), 0, ppi_num());
}

// @brief スキャン方式の擬似外部出力数を得る．
//
// = output_num() + dff_num()
inline
int
TpgNetworkImpl::ppo_num() const
{
  return mOutputNum + mDffNum;
}

// @brief スキャン方式の擬似外部出力を得る．
//< [in] pos 位置番号 ( 0 <= pos < ppo_num() )
inline
const TpgNode*
TpgNetworkImpl::ppo(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < ppo_num() );

  return mPPOArray[pos];
}

// @brief 擬似外部出力のリストを得る．
inline
Array<const TpgNode*>
TpgNetworkImpl::ppo_list() const
{
  return Array<const TpgNode*>(const_cast<const TpgNode**>(mPPOArray), 0, ppo_num());
}

// @brief 故障IDの最大値+1を返す．
inline
int
TpgNetworkImpl::max_fault_id() const
{
  return mFaultNum;
}

// @brief MFFC 数を返す．
inline
int
TpgNetworkImpl::mffc_num() const
{
  return mMffcNum;
}

// @brief FFR 数を返す．
inline
int
TpgNetworkImpl::ffr_num() const
{
  return mFfrNum;
}

// @brief 全代表故障数を返す．
inline
int
TpgNetworkImpl::rep_fault_num() const
{
  return mRepFaultNum;
}

// @brief 代表故障を返す．
//< [in] pos 位置番号 ( 0 <= pos < rep_fault_num() )
inline
const TpgFault*
TpgNetworkImpl::rep_fault(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < rep_fault_num() );

  return mRepFaultArray[pos];
}

// @brief 代表故障のリストを返す．
inline
Array<const TpgFault*>
TpgNetworkImpl::rep_fault_list() const
{
  return Array<const TpgFault*>(mRepFaultArray, 0, rep_fault_num());
}
#endif

END_NAMESPACE_DRUID

#endif // TPGNETWORKIMPL_H
