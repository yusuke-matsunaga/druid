#ifndef SYNCOBJ_H
#define SYNCOBJ_H

/// @file SyncObj.h
/// @brief SyncObj のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <thread>
#include <mutex>
#include <condition_variable>
#include "fsim_nsdef.h"
#include "FsimX.h"


BEGIN_NAMESPACE_DRUID_FSIM

const bool debug = false;

//////////////////////////////////////////////////////////////////////
/// コマンドを表す列挙型
//////////////////////////////////////////////////////////////////////
enum class Cmd: std::uint8_t {
  PPSFP,
  SPPFP_TV,
  SPPFP_AS,
  XSPPFP,
  END
};

inline
std::ostream&
operator<<(
  std::ostream& s,
  Cmd cmd
)
{
  switch ( cmd ) {
  case Cmd::PPSFP:    s << "PPSFP"; break;
  case Cmd::SPPFP_TV: s << "SPPFP_TV"; break;
  case Cmd::SPPFP_AS: s << "SPPFP_AS"; break;
  case Cmd::XSPPFP:   s << "XSPPFP"; break;
  case Cmd::END:      s << "END"; break;
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
/// @class SyncObj SyncObj.h "SyncObj.h"
/// @brief スレッド間の同期を行うオブジェクト
//////////////////////////////////////////////////////////////////////
class SyncObj
{
public:

  /// @brief コンストラクタ
  explicit
  SyncObj(
    SizeType num = 0 ///< [in] 子スレッド数
  ) : mThreadNum{num}
  {
    mReadyCount = 0;
  }

  /// @brief デストラクタ
  ~SyncObj() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief スレッド数を返す．
  SizeType
  thread_num() const
  {
    return mThreadNum;
  }

  /// @brief コマンドを設定する．
  void
  put_sppfp_command(
    const TestVector& tv, ///< [in] テストベクタ
    FsimResultsRep* res   ///< [in] 結果を格納するオブジェクト
  )
  {
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(SPPFP_TV)";
      log(buf.str());
    }
    {
      std::unique_lock lck{mCmdMtx};
      mCmd = Cmd::SPPFP_TV;
      mTv = tv;
      mRes = res;
      mCmdCV.notify_all();
    }
    wait();
  }

  /// @brief コマンドを設定する．
  void
  put_sppfp_command(
    const AssignList& assign_list, ///< [in] 割り当てリスト
    FsimResultsRep* res            ///< [in] 結果を格納するオブジェクト
  )
  {
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(SPPFP_AS)";
      log(buf.str());
    }
    {
      std::unique_lock lck{mCmdMtx};
      mCmd = Cmd::SPPFP_AS;
      mAssignList = assign_list;
      mRes = res;
      mCmdCV.notify_all();
    }
    wait();
  }

  /// @brief コマンドを設定する．
  void
  put_xsppfp_command(
    const AssignList& assign_list, ///< [in] 割り当てリスト
    FsimResultsRep* res            ///< [in] 結果を格納するオブジェクト
  )
  {
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(XSPPFP)";
      log(buf.str());
    }
    {
      std::unique_lock lck{mCmdMtx};
      mCmd = Cmd::XSPPFP;
      mAssignList = assign_list;
      mRes = res;
      mCmdCV.notify_all();
    }
    wait();
  }

  /// @brief コマンドを設定する．
  void
  put_ppsfp_command(
    const std::vector<TestVector>& tv_list,      ///< [in] テストベクタのリスト
    const std::vector<FsimResultsRep*>& res_list ///< [in] 結果を格納するオブジェクトのリスト
  )
  {
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(PPSFP)";
      log(buf.str());
    }
    {
      std::unique_lock lck{mCmdMtx};
      mCmd = Cmd::PPSFP;
      mTvList = tv_list;
      mResList = res_list;
      mCmdCV.notify_all();
    }
    wait();
  }

  /// @brief END コマンドを設定する．
  void
  put_end()
  {
    if ( debug ) {
      std::ostringstream buf;
      buf << "    put_command(END)";
      log(buf.str());
    }
    std::unique_lock lck{mCmdMtx};
    mCmd = Cmd::END;
    mCmdCV.notify_all();
  }

  /// @brief コマンドを取り出す．
  Cmd
  get_command(
    SizeType id
  )
  {
    std::unique_lock lck{mCmdMtx};
    {
      std::lock_guard lck{mReadyMtx};
      ++ mReadyCount;
      if ( debug ) {
	std::ostringstream buf;
	buf << "    waiting for command(" << id << ")"
	    << ", mReadyCount = " << mReadyCount;
	log(buf.str());
      }
      if ( mReadyCount == thread_num() ) {
	// 最後のひとりなら準備完了を通知する．
	mReadyCV.notify_all();
      }
    }
    mCmdCV.wait(lck);
    if ( debug ) {
      std::ostringstream buf;
      buf << "    get_command(" << id << ") => " << mCmd;
      log(buf.str());
    }
    return mCmd;
  }

  /// @brief テストベクタを返す．
  const TestVector&
  testvector()
  {
    return mTv;
  }

  /// @brief 割り当てリストを返す．
  const AssignList&
  assign_list()
  {
    return mAssignList;
  }

  /// @brief テストベクタのリストを返す．
  const std::vector<TestVector>&
  testvector_list()
  {
    return mTvList;
  }

  /// @brief res()/res_list() 用のミューテックスを返す．
  std::mutex&
  res_mutex()
  {
    return mResMtx;
  }

  /// @brief 結果を格納するオブジェクトを返す．
  FsimResultsRep*
  res()
  {
    return mRes;
  }

  /// @brief 結果を格納するオブジェクトのリストを返す．
  const std::vector<FsimResultsRep*>&
  res_list()
  {
    return mResList;
  }

  /// @brief 全ての子スレッドが ready になるのを待つ．
  void
  wait()
  {
    if ( debug ) {
      log("    wait()");
    }
    {
      std::unique_lock lck{mReadyMtx};
      if ( mReadyCount != thread_num() ) {
	mReadyCV.wait(lck);
      }
      mReadyCount = 0;
    }
    if ( debug ) {
      log("    wait() end");
    }
  }

  /// @brief ログを書き出す．
  void
  log(
    const std::string& msg
  )
  {
    std::unique_lock lck{mLogMtx};
    std::cout << msg << std::endl;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 子スレッド数
  SizeType mThreadNum;

  // コマンド
  Cmd mCmd;

  // テストベクタ
  TestVector mTv;

  // テストベクタのリスト
  std::vector<TestVector> mTvList;

  // 割り当てリスト
  AssignList mAssignList;

  // mRes/mResList 用のミューテックス
  std::mutex mResMtx;

  // 結果を格納するオブジェクト
  FsimResultsRep* mRes;

  // 結果を格納するオブジェクトのリスト
  std::vector<FsimResultsRep*> mResList;

  // mCmd 用のミューテックス
  std::mutex mCmdMtx;

  // mCmd 用の条件変数
  std::condition_variable mCmdCV;

  // mReadyCount 用のミューテックス
  std::mutex mReadyMtx;

  // mReadyCount 用の条件変数
  std::condition_variable mReadyCV;

  // ready count
  SizeType mReadyCount{0};

  // ログ用のミューテックス
  std::mutex mLogMtx;

};

END_NAMESPACE_DRUID_FSIM

#endif // SYNCOBJ_H
