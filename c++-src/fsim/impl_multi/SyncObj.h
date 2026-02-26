#ifndef SYNCOBJ_H
#define SYNCOBJ_H

/// @file SyncObj.h
/// @brief SyncObj のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
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
  ) : mNT{num}
  {
    if ( mNT == 0 ) {
      mNT = std::thread::hardware_concurrency();
    }
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
    return mNT;
  }

  /// @brief コマンドを設定する．
  void
  put_sppfp_command(
    const TestVector& tv ///< [in] テストベクタ
  )
  {
    {
      std::unique_lock lck{mCmdMTX};
      mCmd = Cmd::SPPFP_TV;
      mTvPtr = &tv;
      mCmdCV.notify_all();
    }
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(SPPFP_TV)";
      log(buf.str());
    }
    wait();
  }

  /// @brief コマンドを設定する．
  void
  put_sppfp_command(
    const AssignList& assign_list ///< [in] 割り当てリスト
  )
  {
    {
      std::unique_lock lck{mCmdMTX};
      mCmd = Cmd::SPPFP_AS;
      mAssignListPtr = &assign_list;
      mCmdCV.notify_all();
    }
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(SPPFP_AS)";
      log(buf.str());
    }
    wait();
  }

  /// @brief コマンドを設定する．
  void
  put_xsppfp_command(
    const AssignList& assign_list ///< [in] 割り当てリスト
  )
  {
    {
      std::unique_lock lck{mCmdMTX};
      mCmd = Cmd::XSPPFP;
      mAssignListPtr = &assign_list;
      mCmdCV.notify_all();
    }
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(XSPPFP)";
      log(buf.str());
    }
    wait();
  }

  /// @brief コマンドを設定する．
  void
  put_ppsfp_command(
    const std::vector<TestVector>& tv_list ///< [in] テストベクタのリスト
  )
  {
    {
      std::unique_lock lck{mCmdMTX};
      mCmd = Cmd::PPSFP;
      mTvListPtr = &tv_list;
      mCmdCV.notify_all();
    }
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(PPSFP)";
      log(buf.str());
    }
    wait();
  }

  /// @brief END コマンドを設定する．
  void
  put_end()
  {
    std::unique_lock lck{mCmdMTX};
    mCmd = Cmd::END;
    mTvPtr = nullptr;
    mAssignListPtr = nullptr;
    mTvListPtr = nullptr;
    mCmdCV.notify_all();
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(END)";
      log(buf.str());
    }
  }

  /// @brief コマンドを取り出す．
  Cmd
  get_command(
    SizeType id
  )
  {
    std::unique_lock lck2{mCmdMTX};
    {
      std::lock_guard lck1{mReadyMTX};
      ++ mReadyCount;
      if ( mReadyCount == mNT ) {
	mReadyCV.notify_all();
	if ( debug ) {
	  std::ostringstream buf;
	  buf << "get ready(" << id << ")";
	  log(buf.str());
	}
      }
    }
    mCmdCV.wait(lck2);
    if ( debug ) {
      std::ostringstream buf;
      buf << "get_command(" << id << ") => " << mCmd;
      log(buf.str());
    }
    return mCmd;
  }

  /// @brief テストベクタを返す．
  const TestVector&
  testvector()
  {
    return *mTvPtr;
  }

  /// @brief 割り当てリストを返す．
  const AssignList&
  assign_list()
  {
    return *mAssignListPtr;
  }

  /// @brief テストベクタのリストを返す．
  const std::vector<TestVector>&
  testvector_list()
  {
    return *mTvListPtr;
  }

  /// @brief 全ての子スレッドが ready になるのを待つ．
  void
  wait()
  {
    std::unique_lock lck{mReadyMTX};
    if ( debug ) {
      log("wait()");
    }
    if ( mReadyCount != mNT ) {
      mReadyCV.wait(lck);
    }
    mReadyCount = 0;
    if ( debug ) {
      log("wait() end");
    }
  }

  /// @brief ログを書き出す．
  void
  log(
    const std::string& msg
  )
  {
    std::unique_lock lck{mLogMTX};
    std::cout << msg
	      << std::endl;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 子スレッド数
  SizeType mNT;

  // コマンド
  Cmd mCmd;

  // テストベクタ
  const TestVector* mTvPtr{nullptr};

  // テストベクタのリスト
  const std::vector<TestVector>* mTvListPtr{nullptr};

  // 割り当てリスト
  const AssignList* mAssignListPtr{nullptr};

  // mCmd 用のミューテックス
  std::mutex mCmdMTX;

  // mCmd 用の条件変数
  std::condition_variable mCmdCV;

  // mReadyCount 用のミューテックス
  std::mutex mReadyMTX;

  // mReadyCount 用の条件変数
  std::condition_variable mReadyCV;

  // ready count
  SizeType mReadyCount{0};

  // ログ用のミューテックス
  std::mutex mLogMTX;

};

END_NAMESPACE_DRUID_FSIM

#endif // SYNCOBJ_H
