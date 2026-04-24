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
/// @brief コマンドを表す列挙型
//////////////////////////////////////////////////////////////////////
enum class Cmd: std::uint8_t {
  PPSFP, ///< FFRの根のノードの反転イベントを全てのビットを伝える．
  SPPFP, ///< 複数のFFRの根のノードの反転イベントを別々のビットに伝える．
  END    ///< 終了処理
};

/// @brief Cmd の内容をストリーム出力する演算子
inline
std::ostream&
operator<<(
  std::ostream& s,
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
///
/// FsimX 側で put_XXX_command() を呼ぶ．
/// SimEngine 側で get_command() を呼ぶ．
/// コマンドに応じて渡されるパラメータが異なる．
/// - SPPFP: ビット数分の FFR のリスト
/// - PPSFP: FFR
/// - END:   なし
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

  /// @brief SPPFP コマンドを設定する．
  void
  put_sppfp_command(
    const std::vector<const SimFFR*>& ffr_list
  )
  {
    {
      std::unique_lock lck{mCmdMTX};
      mCmd = Cmd::SPPFP;
      mFFRList = ffr_list;
      mNextIndex = 0;
      mCmdCV.notify_all();
    }
    if ( debug ) {
      std::ostringstream buf;
      buf << "put_command(SPPFP)";
      log(buf.str());
    }
    wait();
  }

  /// @brief コマンドを設定する．
  void
  put_ppsfp_command(
    SizeType ntv,
    const std::vector<const SimFFR*>& ffr_list
  )
  {
    {
      std::unique_lock lck{mCmdMTX};
      mCmd = Cmd::PPSFP;
      mNTV = ntv;
      mFFRList = ffr_list;
      mNextIndex = 0;
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

  /// @brief テストベクタ数を返す．
  SizeType
  ntv()
  {
    return mNTV;
  }

  /// @brief FFRを取り出す．
  const SimFFR*
  ffr()
  {
    std::unique_lock lck{mIndexMTX};
    if ( mNextIndex >= mFFRList.size() ) {
      return nullptr;
    }
    auto ans = mFFRList[mNextIndex];
    ++ mNextIndex;
    return ans;
  }

  /// @brief FFRを取り出す．
  std::vector<const SimFFR*>
  ffr_list(
    SizeType max_size
  )
  {
    std::unique_lock lck{mIndexMTX};
    auto n = mFFRList.size();
    if ( mNextIndex >= n ) {
      return {};
    }
    std::vector<const SimFFR*> ans_list;
    ans_list.reserve(max_size);
    n = std::min(n, mNextIndex + max_size);
    while ( mNextIndex < n ) {
      auto ffr = mFFRList[mNextIndex];
      ans_list.push_back(ffr);
      ++ mNextIndex;
    }
    return ans_list;
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

  // テストベクタ数
  SizeType mNTV;

  // FFR のリスト
  std::vector<const SimFFR*> mFFRList;

  // mFFRList 中の次のインデックス
  SizeType mNextIndex;

  // mCmd 用のミューテックス
  std::mutex mCmdMTX;

  // mCmd 用の条件変数
  std::condition_variable mCmdCV;

  // mNextIndex 用のミューテックス
  std::mutex mIndexMTX;

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
