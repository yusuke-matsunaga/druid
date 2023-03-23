
/// @file mf_comp.cc
/// @brief DtpgTest を使ったサンプルプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "TpgNetwork.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "TpgFaultMgr.h"
#include "TpgNode.h"
#include "MF_FaultComp.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_DRUID

BEGIN_NONAMESPACE

void
ffr_decomp(
  const TpgNetwork& network,
  TpgFaultMgr& fmgr,
  const TpgNode* root,
  int limit,
  SatSolverType solver_type,
  int& comp_bits
)
{
  vector<const TpgNode*> node_stack;
  vector<const TpgNode*> node_list;
  vector<const TpgFault*> fault_list;
  unordered_set<int> node_mark;

  node_stack.push_back(root);
  while ( !node_stack.empty() ) {
    auto node = node_stack.back();
    int nf1 = fmgr.node_fault_list(node->id()).size();
    ASSERT_COND( nf1 <= limit );
    if ( fault_list.size() + nf1 > limit ) {
      // 容量オーバー
      break;
    }
    else {
      for ( auto f: fmgr.node_fault_list(node->id()) ) {
	fault_list.push_back(f);
      }
      node_stack.pop_back();
      node_list.push_back(node);
      node_mark.emplace(node->id());

      for ( auto inode: node->fanin_list() ) {
	if ( inode->ffr_root() != inode ) {
	  node_stack.push_back(inode);
	}
      }
    }
  }
  vector<const TpgNode*> input_list;
  for ( auto node1: node_list ) {
    if ( node1->is_ppi() ) {
      input_list.push_back(node1);
    }
    else {
      for ( auto inode: node1->fanin_list() ) {
	if ( node_mark.count(inode->id()) == 0 ) {
	  input_list.push_back(inode);
	  node_mark.emplace(inode->id());
	}
      }
    }
  }
  //cout << " NF1: " << fault_list.size() << endl;
  vector<vector<const TpgFault*>> faults_list =
    MF_FaultComp::get_faults_list(network, root, input_list, fault_list, solver_type);
  int cnum = faults_list.size();
  int cbits1 = 1;
  while ( (1 << cbits1) <= cnum ) {
    ++ cbits1;
  }
  comp_bits += cbits1;
  //cout << " NC1: " << cbits1 << endl;

  for ( auto root1: input_list ) {
    if ( root1->is_ppi() || root1->ffr_root() == root1 ) {
      continue;
    }
    ffr_decomp(network, fmgr, root1, limit, solver_type, comp_bits);
  }
}

END_NONAMESPACE

const char* argv0 = "";

void
usage()
{
  cerr << "USAGE: " << argv0 << " --blif|--iscas89 <limit> <file>" << endl;
}

int
mf_comp(
  int argc,
  char** argv
)
{
  string sat_type;
  string sat_option;
  ostream* sat_outp = nullptr;

  string format = "blif";

  bool dump = false;

  bool verbose = false;

  argv0 = argv[0];

  int pos = 1;
  for ( ; pos < argc; ++ pos) {
    if ( argv[pos][0] == '-' ) {
      if ( strcmp(argv[pos], "--blif") == 0 ) {
	format = "blif";
      }
      else if ( strcmp(argv[pos], "--iscas89") == 0 ) {
	format = "iscas89";
      }
      else if ( strcmp(argv[pos], "--dump") == 0 ) {
	dump = true;
      }
      else if ( strcmp(argv[pos], "--verbose") == 0 ) {
	verbose = true;
      }
      else {
	cerr << argv[pos] << ": illegal option" << endl;
	usage();
	return -1;
      }
    }
    else {
      break;
    }
  }

  if ( pos != argc - 2 ) {
    usage();
    return -1;
  }

  int limit = atoi(argv[pos]);

  string filename = argv[pos + 1];
  auto network = TpgNetwork::read_network(filename, format);

  if ( dump ) {
    network.print(cout);
  }

  TpgFaultMgr fmgr;
  fmgr.gen_fault_list(network, FaultType::StuckAt);

  SatSolverType solver_type(sat_type, sat_option, sat_outp);

  int orig_bits = 0;
  int comp_bits = 0;
  for ( SizeType ffr_id = 0; ffr_id < network.ffr_num(); ++ ffr_id ) {
    auto ffr = network.ffr(ffr_id);
    int obits1 = fmgr.ffr_fault_list(ffr_id).size();
    int cbits1 = obits1;

    if ( verbose ) {
      cout << "NF:   " << obits1 << endl;
    }

    SizeType nf = fmgr.ffr_fault_list(ffr_id).size();
    if ( nf <= limit ) {
      int ni = ffr.input_num();
      vector<const TpgNode*> input_list(ni);
      for ( int i: Range(ni) ) {
	auto node = ffr.input(i);
	input_list[i] = node;
      }
      vector<const TpgFault*> fault_list;
      fault_list.reserve(nf);
      for ( auto f: fmgr.ffr_fault_list(ffr_id) ) {
	fault_list.push_back(f);
      }
      auto faults_list = MF_FaultComp::get_faults_list(network, ffr.root(), input_list, fault_list, solver_type);
      int cnum = faults_list.size();
      cbits1 = 1;
      while ( (1 << cbits1) <= cnum ) {
	++ cbits1;
      }
    }
    else {
      cbits1 = 0;
      ffr_decomp(network, fmgr, ffr.root(), limit, solver_type, cbits1);
    }

    if ( verbose ) {
      cout << "NC:   " << cbits1 << endl
	   << endl;
    }

    orig_bits += obits1;
    comp_bits += cbits1;
  }

  cout << "Total Fault Bits:      " << orig_bits << endl
       << "Compressed Fault Bits: " << comp_bits << endl;
  return 0;
}

END_NAMESPACE_DRUID


int
main(int argc,
     char** argv)
{
  return DRUID_NAMESPACE::mf_comp(argc, argv);
}
