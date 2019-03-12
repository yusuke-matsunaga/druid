#ifndef TPGNETWORKIMPL_H
#define TPGNETWORKIMPL_H

/// @file TpgNetworkImpl.h
/// @brief TpgNetworkImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "druid.h"

#include "ym/bnet.h"
#include "ym/clib.h"
#include "ym/logic.h"
#include "ym/Array.h"
#include "ym/SimpleAlloc.h"


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
  node_num() const;

  /// @brief ノードを得る．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  ///
  /// @code
  /// node = network.node(node->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  node(int id) const;

  /// @brief 全ノードのリストを得る．
  Array<const TpgNode*>
  node_list() const;

  /// @brief ノード名を得る．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  const char*
  node_name(int id) const;

  /// @brief 外部入力数を得る．
  int
  input_num() const;

  /// @brief 外部入力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
  ///
  /// @code
  /// node = network.input(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  input(int pos) const;

  /// @brief 外部入力ノードのリストを得る．
  Array<const TpgNode*>
  input_list() const;

  /// @brief 外部出力数を得る．
  int
  output_num() const;

  /// @brief 外部出力ノードを得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  ///
  /// @code
  /// node = network.output(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output(int pos) const;

  /// @brief 外部出力ノードのリストを得る．
  Array<const TpgNode*>
  output_list() const;

  /// @brief TFIサイズの降順で整列した順番で外部出力ノードを取り出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
  ///
  /// @code
  /// node = network.output2(node->output_id2())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  output2(int pos) const;

  /// @brief スキャン方式の擬似外部入力数を得る．
  ///
  /// = input_num() + dff_num()
  int
  ppi_num() const;

  /// @brief スキャン方式の擬似外部入力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ppi_num() )
  ///
  /// @code
  /// node = network.ppi(node->input_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppi(int pos) const;

  /// @brief 擬似外部入力のリストを得る．
  Array<const TpgNode*>
  ppi_list() const;

  /// @brief スキャン方式の擬似外部出力数を得る．
  ///
  /// = output_num() + dff_num()
  int
  ppo_num() const;

  /// @brief スキャン方式の擬似外部出力を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < ppo_num() )
  ///
  /// @code
  /// node = network.ppo(node->output_id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgNode*
  ppo(int pos) const;

  /// @brief 擬似外部出力のリストを得る．
  Array<const TpgNode*>
  ppo_list() const;

  /// @brief MFFC 数を返す．
  int
  mffc_num() const;

  /// @brief MFFC を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < mffc_num() )
  const TpgMFFC&
  mffc(int pos) const;

  /// @brief MFFC のリストを得る．
  Array<const TpgMFFC>
  mffc_list() const;

  /// @brief FFR 数を返す．
  int
  ffr_num() const;

  /// @brief FFR を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < ffr_num() )
  const TpgFFR&
  ffr(int pos) const;

  /// @brief FFR のリストを得る．
  Array<const TpgFFR>
  ffr_list() const;

  /// @brief DFF数を得る．
  int
  dff_num() const;

  /// @brief DFF を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < dff_num() )
  ///
  /// @code
  /// dff = network.dff(dff->id())
  /// @endcode
  /// の関係が成り立つ．
  const TpgDff&
  dff(int pos) const;

  /// @brief DFF のリストを得る．
  Array<const TpgDff>
  dff_list() const;

  /// @brief 故障IDの最大値+1を返す．
  int
  max_fault_id() const;

  /// @brief 全代表故障数を返す．
  int
  rep_fault_num() const;

  /// @brief 代表故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < rep_fault_num() )
  const TpgFault*
  rep_fault(int pos) const;

  /// @brief 代表故障のリストを返す．
  Array<const TpgFault*>
  rep_fault_list() const;

  /// @brief ノードに関係した代表故障数を返す．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  int
  node_rep_fault_num(int id) const;

  /// @brief ノードに関係した代表故障を返す．
  /// @param[in] id ID番号 ( 0 <= id < node_num() )
  /// @param[in] pos 位置番号 ( 0 <= pos < node_rep_fault_num(id) )
  const TpgFault*
  node_rep_fault(int id,
		 int pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定するための関数
  //////////////////////////////////////////////////////////////////////

  /// @brief BnNetwork から内容を設定する．
  /// @param[in] network 設定元のネットワーク
  void
  set(const BnNetwork& network);

  /// @brief サイズを設定する．
  void
  set_size(int input_num,
	   int output_num,
	   int dff_num,
	   int node_num);


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
    InodeInfo(TpgNode* node = nullptr,
	      int pos = 0) :
      mNode(node),
      mPos(pos)
    {
    }

    void
    set(TpgNode* node,
	int pos)
    {
      mNode = node;
      mPos = pos;
    }

    TpgNode* mNode;
    int mPos;
  };


public:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をクリアする．
  void
  clear();

  /// @brief 入力ノードを生成する．
  /// @param[in] iid 入力の番号
  /// @param[in] name ノード名
  /// @param[in] fanout_num ファンアウト数
  /// @return 生成したノードを返す．
  TpgNode*
  make_input_node(int iid,
		  const string& name,
		  int fanout_num);

  /// @brief 出力ノードを生成する．
  /// @param[in] oid 出力の番号
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_output_node(int oid,
		   const string& name,
		   TpgNode* inode);

  /// @brief DFFの入力ノードを生成する．
  /// @param[in] oid 出力の番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_input_node(int oid,
		      const TpgDff* dff,
		      const string& name,
		      TpgNode* inode);

  /// @brief DFFの出力ノードを生成する．
  /// @param[in] iid 入力の番号
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] fanout_num ファンアウト数
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_output_node(int iid,
		       const TpgDff* dff,
		       const string& name,
		       int fanout_num);

  /// @brief DFFのクロック端子を生成する．
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_clock_node(const TpgDff* dff,
		      const string& name,
		      TpgNode* inode);

  /// @brief DFFのクリア端子を生成する．
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_clear_node(const TpgDff* dff,
		      const string& name,
		      TpgNode* inode);

  /// @brief DFFのプリセット端子を生成する．
  /// @param[in] dff 接続しているDFF
  /// @param[in] name ノード名
  /// @param[in] inode 入力のノード
  /// @return 生成したノードを返す．
  TpgNode*
  make_dff_preset_node(const TpgDff* dff,
		       const string& name,
		       TpgNode* inode);

  /// @brief 論理ノードを生成する．
  /// @param[in] name ノード名
  /// @param[in] node_info 論理関数の情報
  /// @param[in] fanin_list ファンインのリスト
  /// @param[in] fanout_num ファンアウト数
  /// @param[out] connection_list 接続リスト
  /// @return 生成したノードを返す．
  TpgNode*
  make_logic_node(const string& name,
		  const TpgGateInfo* node_info,
		  const vector<TpgNode*>& fanin_list,
		  int fanout_num,
		  vector<pair<int, int>>& connection_list);

  /// @brief 論理式から TpgNode の木を生成する．
  /// @param[in] name ノード名
  /// @param[in] expr 式
  /// @param[in] leaf_nodes 式のリテラルに対応するノードの配列
  /// @param[in] inode_array ファンインの対応関係を収める配列
  /// @param[in] fanout_num ファンアウト数
  /// @param[out] connection_list 接続リスト
  /// @return 生成したノードを返す．
  ///
  /// leaf_nodes は 変数番号 * 2 + (0/1) に
  /// 該当する変数の肯定/否定のリテラルが入っている．
  TpgNode*
  make_cplx_node(const string& name,
		 const Expr& expr,
		 const vector<TpgNode*>& leaf_nodes,
		 vector<InodeInfo>& inode_array,
		 int fanout_num,
		 vector<pair<int, int>>& connection_list);

  /// @brief 組み込み型の論理ゲートを生成する．
  /// @param[in] name ノード名
  /// @param[in] type ゲートの型
  /// @param[in] fanin_list ファンインのリスト
  /// @param[in] fanout_num ファンアウト数
  /// @param[out] connection_list 接続リスト
  /// @return 生成したノードを返す．
  TpgNode*
  make_prim_node(const string& name,
		 GateType type,
		 const vector<TpgNode*>& fanin_list,
		 int fanout_num,
		 vector<pair<int, int>>& connection_list);

  /// @brief バッファを生成する．
  /// @param[in] name ノード名
  /// @param[in] fanin ファンインのノード
  /// @param[in] fanout_num ファンアウト数
  /// @param[out] connection_list 接続リスト
  /// @return 生成したノードを返す．
  TpgNode*
  make_buff_node(const string& name,
		 TpgNode* fanin,
		 int fanout_num,
		 vector<pair<int, int>>& connection_list);

  /// @brief インバーターを生成する．
  /// @param[in] name ノード名
  /// @param[in] fanin ファンインのノード
  /// @param[in] fanout_num ファンアウト数
  /// @param[out] connection_list 接続リスト
  /// @return 生成したノードを返す．
  TpgNode*
  make_not_node(const string& name,
		TpgNode* fanin,
		int fanout_num,
		vector<pair<int, int>>& connection_list);

  /// @brief 出力の故障を作る．
  /// @param[in] name 故障位置のノード名
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] node 故障位置のノード
  void
  new_ofault(const char* name,
	     int val,
	     const TpgNode* node);

  /// @brief 入力の故障を作る．
  /// @param[in] name 故障位置のノード名
  /// @param[in] ipos 故障位置のファンイン番号
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] inode_info TpgNode 上のノードの情報
  /// @param[in] rep 代表故障
  ///
  /// プリミティブ型の場合は ipos と inode_pos は同一だが
  /// 複合型の場合には異なる．
  void
  new_ifault(const char* name,
	     int ipos,
	     int val,
	     const InodeInfo& inode_info,
	     TpgFault* rep);

  /// @brief 出力の故障を得る．
  /// @param[in] id ノードID ( 0 <= id < node_num() )
  /// @param[in] val 故障値 ( 0 / 1 )
  TpgFaultBase*
  _node_output_fault(int id,
		     int val);

  /// @brief 入力の故障を得る．
  /// @param[in] id ノードID ( 0 <= id < node_num() )
  /// @param[in] val 故障値 ( 0 / 1 )
  /// @param[in] pos 入力の位置番号
  TpgFaultBase*
  _node_input_fault(int id,
		    int val,
		    int pos);


  /// @brief 代表故障を設定する．
  /// @param[in] node 対象のノード
  int
  set_rep_faults(const TpgNode* node);

  /// @brief FFR の情報を設定する．
  /// @param[in] root FFR の根のノード
  /// @param[in] ffr 対象の FFR
  void
  set_ffr(const TpgNode* root,
	  TpgFFR* ffr);

  /// @brief MFFC の情報を設定する．
  /// @param[in] root MFFCの根のノード
  /// @param[in] mffc 対象のMFFC
  void
  set_mffc(const TpgNode* root,
	   TpgMFFC* mffc);

  /// @brief ノードのリストからノードの配列を作る．
  const TpgNode**
  make_node_array(const vector<const TpgNode*>& node_list);

  /// @brief 故障リストから故障の配列を作る．
  const TpgFault**
  make_fault_array(const vector<const TpgFault*>& fault_list);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // TpgNetwork 関係のメモリ確保を行なうオブジェクト
  SimpleAlloc mAlloc;

  // 外部入力数
  int mInputNum;

  // 外部出力数
  int mOutputNum;

  // DFF数
  int mDffNum;

  // DFFの実体の配列
  TpgDff* mDffArray;

  // ノード数(mNodeArrayの要素数)
  int mNodeNum;

  // ノードの配列
  TpgNode** mNodeArray;

  // ノードの付加情報の配列
  AuxNodeInfo* mAuxInfoArray;

  // PPIノードの配列
  TpgNode** mPPIArray;

  // PPOノードの配列
  TpgNode** mPPOArray;

  // TFI サイズの降順に整列したPPOノードの配列
  TpgNode** mPPOArray2;

  // MFFC 数
  int mMffcNum;

  // MFFC の本体の配列
  TpgMFFC* mMffcArray;

  // FFR 数
  int mFfrNum;

  // FFR の本体の配列
  TpgFFR* mFfrArray;

  // 全故障数
  int mFaultNum;

  // 全代表故障数
  int mRepFaultNum;

  // 代表故障の配列
  const TpgFault** mRepFaultArray;

};


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
// @param[in] pos 位置番号 ( 0 <= pos < input_num() )
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
// @param[in] pos 位置番号 ( 0 <= pos < output_num() )
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
// @param[in] pos 位置番号 ( 0 <= pos < ppi_num() )
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
// @param[in] pos 位置番号 ( 0 <= pos < ppo_num() )
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
// @param[in] pos 位置番号 ( 0 <= pos < rep_fault_num() )
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

END_NAMESPACE_DRUID

#endif // TPGNETWORKIMPL_H
