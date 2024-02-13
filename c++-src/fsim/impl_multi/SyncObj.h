#ifndef SYNCOBJ_H
#define SYNCOBJ_H

/// @file SyncObj.h
/// @brief SyncObj のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <thread>
#include "fsim_nsdef.h"


BEGIN_NAMESPACE_DRUID_FSIM

class InputVals;

const bool debug = false;

//////////////////////////////////////////////////////////////////////
/// コマンドを表す列挙型
//////////////////////////////////////////////////////////////////////
enum class Cmd: std::uint8_t {
  PPSFP,
  SPPFP,
  END
};

inline
ostream&
operator<<(
  ostream& s,
  Cmd cmd
)
{
  switch ( cmd ) {
  case Cmd::PPSFP: s << "PPSFP"; break;
  case Cmd::SPPFP: s << "SPPFP"; break;
  case Cmd::END:   s << "END"; break;
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
  put_command(
    Cmd cmd,            ///< [in] コマンド
    const InputVals& iv ///< [in] 入力値
  )
  {
    std::unique_lock lck{mCmdMTX};
    mCmd = cmd;
    mIV = &iv;
    mNextId = 0;
    mCmdCV.notify_all();
    if ( debug ) {
      ostringstream buf;
      buf << "put_command(" << cmd << ")";
      log(buf.str());
    }
  }

  /// @brief END コマンドを設定する．
  void
  put_end()
  {
    std::unique_lock lck{mCmdMTX};
    mCmd = Cmd::END;
    mIV = nullptr;
    mNextId = 0;
    mCmdCV.notify_all();
    if ( debug ) {
      ostringstream buf;
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
	  ostringstream buf;
	  buf << "get ready(" << id << ")";
	  log(buf.str());
	}
      }
    }
    mCmdCV.wait(lck2);
    if ( debug ) {
      ostringstream buf;
      buf << "get_command(" << id << ") => " << mCmd;
      log(buf.str());
    }
    return mCmd;
  }

  /// @brief 入力値を返す．
  const InputVals&
  input_vals()
  {
    return *mIV;
  }

  /// @brief カウンタ値を取り出す．
  SizeType
  get_id()
  {
    std::unique_lock lck{mIdMutex};
    auto id = mNextId;
    ++ mNextId;
    return id;
  }

  /// @brief 全ての子スレッドが ready になるのを待つ．
  void
  wait()
  {
    std::unique_lock lck{mReadyMTX};
    if ( debug ) {
      log("wait()");
    }
    mReadyCV.wait(lck);
    mReadyCount = 0;
    if ( debug ) {
      log("wait() end");
    }
  }

  /// @brief ログを書き出す．
  void
  log(
    const string& msg
  )
  {
    std::unique_lock lck{mLogMTX};
    cout << msg << endl;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 子スレッド数
  SizeType mNT;

  // コマンド
  Cmd mCmd;

  // 入力値
  const InputVals* mIV;

  // mCmd 用のミューテックス
  std::mutex mCmdMTX;

  // mCmd 用の条件変数
  std::condition_variable mCmdCV;

  // カウンタ
  SizeType mNextId;

  // mNextId 用のミューテックス
  std::mutex mIdMutex;

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
