
/// @file NaiveReduce.cc
/// @brief NaiveReduce の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2026 Yusuke Matsunaga
/// All rights reserved.

#include "NaiveReduce.h"
#include "fsim/Fsim.h"
#include "dtpg/NaiveDualEngine.h"
#include "ym/HeapTree.h"



BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

std::string
time_str(
  Timer& timer
)
{
  std::ostringstream buf;
  buf << std::setw(11)
      << std::fixed
      << std::setprecision(2)
      << timer.get_time()
      << "ms";
  return buf.str();
}

// count_array に従って比較を行うファンクタクラス
class FaultComp
{
public:

  FaultComp(
    SizeType size
  ) : count_array(size, 0)
  {
  }

  int
  operator()(
    SizeType id1,
    SizeType id2
  ) const
  {
    auto c1 = count_array[id1];
    auto c2 = count_array[id2];
    if ( c1 < c2 ) {
      return -1;
    }
    if ( c1 > c2 ) {
      return 1;
    }
    return 0;
  }

  void
  inc_count(
    SizeType id
  )
  {
    ++ count_array[id];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  std::vector<SizeType> count_array;

};

// 2つの故障間の故障シミュレーションの結果を表すクラス
class PatMgr
{
public:

  /// @brief コンストラクタ
  PatMgr(
    const TpgFaultList& fault_list, ///< [in] 対象の故障リスト
    SizeType size                   ///< [in] 最大サイズ
  ) : mFaultList{fault_list},
      mSize{size},
      mArray(mSize * mSize, 0)
  {
  }

  /// @brief デストラクタ
  ~PatMgr() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障シミュレーションの結果を登録する．
  /// @return 変化があったら true を返す．
  bool
  add(
    const FsimResults& res ///< [in] 故障シミュレーションの結果
  );

  /// @brief 等価な可能性のある故障のリストを返す．
  TpgFaultList
  eqcand_list(
    const TpgFault& fault
  ) const;

  /// @brief 支配する可能性のある故障のリストを返す．
  TpgFaultList
  domcand_list(
    const TpgFault& fault
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// 故障対に対するインデックスを計算する．
  SizeType
  _index(
    const TpgFault& fault1,
    const TpgFault& fault2
  ) const
  {
    return fault1.id() * mSize + fault2.id();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 対象の故障のリスト
  TpgFaultList mFaultList;

  // サイズ
  SizeType mSize;

  // 2つの故障間の関係を表すビットパタンの配列
  // サイズは mSize * mSize
  // 0: 故障1が故障2を支配する可能性なし(1, 0 のパタンあり)
  std::vector<bool> mArray;

};

// @brief 故障シミュレーションの結果を登録する．
bool
PatMgr::add(
  const FsimResults& res
)
{
  auto ntv = res.tv_num();
  // 各故障に対するビットパタンを作る．
  std::vector<PackedVal> pat_array(mSize, 0);
  for ( SizeType i = 0; i < ntv; ++ i ) {
    PackedVal pat = 1ULL << i;
    for ( auto fault: res.fault_list(i) ) {
      pat_array[fault.id()] |= pat;
    }
  }

  auto nf = mFaultList.size();
  bool change = false;
  for ( SizeType i1 = 0; i1 < nf - 1; ++ i1 ) {
    auto fault1 = mFaultList[i1];
    auto pat1 = pat_array[fault1.id()];
    for ( SizeType i2 = i1 + 1; i2 < nf; ++ i2 ) {
      auto fault2 = mFaultList[i2];
      auto pat2 = pat_array[fault2.id()];
      if ( (pat1 & ~pat2) != PV_ALL0 ) {
	auto idx1 = _index(fault1, fault2);
	if ( !mArray[idx1] ) {
	  change = true;
	}
	mArray[idx1] = true;
      }
      if ( (pat2 & ~pat1) != PV_ALL0 ) {
	auto idx2 = _index(fault2, fault1);
	if ( !mArray[idx2] ) {
	  change = true;
	}
	mArray[idx2] = true;
      }
    }
  }
  return change;
}

// @brief 等価故障の可能性のあるリストを返す．
TpgFaultList
PatMgr::eqcand_list(
  const TpgFault& fault
) const
{
  TpgFaultList ans_list;
  for ( auto fault2: mFaultList ) {
    if ( fault2 == fault ) {
      continue;
    }
    auto idx1 = _index(fault, fault2);
    auto idx2 = _index(fault2, fault);
    if ( !mArray[idx1] && !mArray[idx2] ) {
      ans_list.push_back(fault2);
    }
  }
  return ans_list;
}

// @brief 支配する可能性のある故障のリストを返す．
TpgFaultList
PatMgr::domcand_list(
  const TpgFault& fault
) const
{
  TpgFaultList ans_list;
  for ( auto fault2: mFaultList ) {
    if ( fault2 == fault ) {
      continue;
    }
    auto idx = _index(fault, fault2);
    if ( !mArray[idx] ) {
      ans_list.push_back(fault2);
    }
  }
  return ans_list;
}

END_NONAMESPACE


// @brief 故障グループの細分化を行ってから支配関係を調べる．
void
NaiveReduce::run(
  FaultInfo& fault_info,
  const ConfigParam& option
)
{
  SizeType NO_CHANGE_LIMIT = option.get_int_elem("no_change_limit", 1000);
  SizeType BATCH_SIZE = std::min(64, option.get_int_elem("batch_size", 64));
  SizeType TIME_LIMIT = option.get_int_elem("time_limit", 0);
  auto verbose = option.get_bool_elem("verbose", false);

  Timer timer;
  timer.start();

  SizeType check_count = 0;
  SizeType succ_count = 0;
  auto fault_list = fault_info.rep_fault_list();

  auto network = fault_list.network();

  // 故障シミュレータ
  auto fsim_option = option.get_param("fsim");
  Fsim fsim(fault_list, fsim_option);

  // 故障番号をキーにして検出回数で比較する関数オブジェクト
  FaultComp comp(network.max_fault_id());
  // 故障番号を入れるヒープ木
  auto heap = HeapTree<SizeType, FaultComp>(comp);
  for ( auto fault: fault_list ) {
    heap.put_item(fault.id());
  }

  Timer timer1;
  timer1.start();

  Timer fsim_timer;
  SizeType tv_count = 0;
  std::mt19937 randgen;
  SizeType no_change = 0;
  PatMgr patmgr(fault_list, network.max_fault_id());
  while ( no_change < NO_CHANGE_LIMIT ) {
    std::vector<TestVector> tv_list(BATCH_SIZE);
    for ( SizeType base = 0; base < BATCH_SIZE; ++ base ) {
      if ( heap.empty() ) {
	// ランダムパタンを作る．
	auto tv1 = TestVector(network);
	tv1.set_from_random(randgen);
	tv_list[base] = tv1;
      }
      else {
	// 検出回数が最小の故障を一つ選ぶ．
	auto fid = heap.get_min();
	auto min_fault = network.fault(fid);
	// この故障を検出するテストベクタを用いて故障シミュレーションを行う．
	auto tv1 = fault_info.testvector(min_fault);
	// X の部分はランダムに埋める．
	tv1.fix_x_from_random(randgen);
	tv_list[base] = tv1;
      }
    }
    fsim_timer.start();
    auto res = fsim.run_multi(tv_list, true);
    fsim_timer.stop();
    tv_count += BATCH_SIZE;

    auto change = patmgr.add(res);
    if ( change ) {
      no_change = 0;
    }
    else {
      no_change += BATCH_SIZE;
    }
  }
  timer1.stop();
  if ( verbose ) {
    std::cout << "Total # of patterns:    " << std::setw(8) << std::right << tv_count << std::endl;
  }

  Timer timer_phase1;
  timer_phase1.start();
  for ( auto fault1: fault_list ) {
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    for ( auto fault2: patmgr.eqcand_list(fault1) ) {
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2, option);
      ++ check_count;
      auto res10 = engine.solve(true, false, TIME_LIMIT);
      auto res01 = engine.solve(false, true, TIME_LIMIT);
      if ( res10 == SatBool3::False && res01 == SatBool3::False ) {
	// fault1 と fault2 は等価
	fault_info.set_rep(fault2, fault1);
	++ succ_count;
      }
    }
  }
  timer_phase1.stop();
  if ( verbose ) {
    std::cout << std::endl
	      << "Equivalence check end:    " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl;
  }

  Timer timer_phase2;
  timer_phase2.start();
  check_count = 0;
  succ_count = 0;
  for ( auto fault1: fault_list ) {
    if ( !fault_info.is_rep(fault1) ) {
      continue;
    }
    for ( auto fault2: patmgr.domcand_list(fault1) ) {
      if ( !fault_info.is_rep(fault2) ) {
	continue;
      }
      NaiveDualEngine engine(fault1, fault2, option);
      ++ check_count;
      auto res10 = engine.solve(true, false, TIME_LIMIT);
      if ( res10 == SatBool3::False ) {
	// fault2 は fault1 に支配されている．
	fault_info.set_dominator(fault2, fault1);
	++ succ_count;
      }
    }
  }
  timer_phase2.stop();

  timer.stop();
  if ( verbose ) {
    std::cout << std::endl
	      << "Dominance check end:      " << std::endl
	      << "Total checks:             " << std::setw(8) << std::right << check_count << std::endl
	      << "Total succeeds:           " << std::setw(8) << std::right << succ_count << std::endl
	      << "-------------------------------------------------------" << std::endl
	      << "Screening Time:           " << time_str(timer1) << std::endl
	      << " (Fsim Time):             " << time_str(fsim_timer) << std::endl
	      << "Phase1 Time:              " << time_str(timer_phase1) << std::endl
	      << "Phase2 Time:              " << time_str(timer_phase2) << std::endl
	      << "Total CPU Time:           " << time_str(timer) << std::endl;
  }
}

END_NAMESPACE_DRUID
