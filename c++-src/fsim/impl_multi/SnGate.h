#ifndef SNGATE_H
#define SNGATE_H

/// @file SnGate.h
/// @brief SnGate のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "SimNode.h"


BEGIN_NAMESPACE_DRUID_FSIM

//////////////////////////////////////////////////////////////////////
/// @class SnGate SimNode.h
/// @brief 多入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  );

  /// @brief デストラクタ
  ~SnGate();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const override;

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  fanin(
    SizeType pos
  ) const override;

  /// @brief 内容をダンプする．
  void
  dump(
    std::ostream& s
  ) const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンイン数を得る．
  SizeType
  _fanin_num() const
  {
    return mFaninNum;
  }

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  _fanin(
    SizeType pos
  ) const
  {
    return mFanins[pos];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 入力数
  SizeType mFaninNum;

  // ファンインのノード番号の配列
  SizeType* mFanins;

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate1 SimNode.h
/// @brief 1入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate1 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate1(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  );

  /// @brief デストラクタ
  ~SnGate1();


public:

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const override;

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  fanin(
    SizeType pos
  ) const override;

  /// @brief 内容をダンプする．
  void
  dump(
    std::ostream& s
  ) const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ファンインのノード番号を得る．
  SizeType
  _fanin() const
  {
    return mFanin;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンイン
  SizeType mFanin;

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate2 SimNode.h
/// @brief 2入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate2 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate2(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  );

  /// @brief デストラクタ
  ~SnGate2();


public:

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const override;

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  fanin(
    SizeType pos
  ) const override;

  /// @brief 内容をダンプする．
  void
  dump(
    std::ostream& s
  ) const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  _fanin(
    SizeType pos
  ) const
  {
    return mFanins[pos];
  }

  /// @brief _calc_gobs の下請け関数
  FSIM_VALTYPE
  _get_sideval(
    const std::vector<FSIM_VALTYPE>& val_array, ///< [in] 値の配列
    SizeType pos                           ///< [in] 対象の入力位置
  ) const
  {
    SizeType pos0 = pos ^ 1;
    return val_array[_fanin(pos0)];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  SizeType mFanins[2];

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate3 SimNode.h
/// @brief 3入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate3 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate3(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  );

  /// @brief デストラクタ
  ~SnGate3();


public:

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const override;

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  fanin(
    SizeType pos
  ) const override;

  /// @brief 内容をダンプする．
  void
  dump(
    std::ostream& s
  ) const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  _fanin(
    SizeType pos
  ) const
  {
    return mFanins[pos];
  }

  /// @brief _calc_gobs のための下請け関数
  void
  _get_sideval(
    const std::vector<FSIM_VALTYPE>& val_array, ///< [in] 値の配列
    SizeType pos,                          ///< [in] 対象の入力位置
    FSIM_VALTYPE& val0,                    ///< [out] それ以外の入力値0
    FSIM_VALTYPE& val1                     ///< [out] それ以外の入力値1
  ) const
  {
    SizeType pos0;
    SizeType pos1;
    switch ( pos ) {
    case 0: pos0 = 1; pos1 = 2; break;
    case 1: pos0 = 0; pos1 = 2; break;
    case 2: pos0 = 0; pos1 = 1; break;
    default:
      throw std::logic_error{"never be reached"};
    }
    val0 = val_array[_fanin(pos0)];
    val1 = val_array[_fanin(pos1)];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  SizeType mFanins[3];

};


//////////////////////////////////////////////////////////////////////
/// @class SnGate4 SimNode.h
/// @brief 4入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////
class SnGate4 :
  public SimNode
{
protected:

  /// @brief コンストラクタ
  SnGate4(
    SizeType id,
    SizeType level,
    const std::vector<SizeType>& inputs
  );

  /// @brief デストラクタ
  ~SnGate4();


public:

  /// @brief ファンイン数を得る．
  SizeType
  fanin_num() const override;

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  fanin(
    SizeType pos
  ) const override;

  /// @brief 内容をダンプする．
  void
  dump(
    std::ostream& s
  ) const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから呼ばれる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief pos 番めのファンインのノード番号を得る．
  SizeType
  _fanin(
    SizeType pos
  ) const
  {
    return mFanins[pos];
  }

  /// @brief _calc_gobs のための下請け関数
  void
  _get_sideval(
    const std::vector<FSIM_VALTYPE>& val_array, ///< [in] 値の配列
    SizeType pos,                          ///< [in] 対象の入力位置
    FSIM_VALTYPE& val0,                    ///< [out] それ以外の入力値0
    FSIM_VALTYPE& val1,                    ///< [out] それ以外の入力値1
    FSIM_VALTYPE& val2                     ///< [out] それ以外の入力値2
  ) const
  {
    SizeType pos0;
    SizeType pos1;
    SizeType pos2;
    switch ( pos ) {
    case 0: pos0 = 1; pos1 = 2; pos2 = 3; break;
    case 1: pos0 = 0; pos1 = 2; pos2 = 3; break;
    case 2: pos0 = 0; pos1 = 1; pos2 = 3; break;
    case 3: pos0 = 0; pos1 = 1; pos2 = 2; break;
    default:
      throw std::logic_error{"never be reached"};
    }
    val0 = val_array[_fanin(pos0)];
    val1 = val_array[_fanin(pos1)];
    val2 = val_array[_fanin(pos2)];
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ファンインの配列
  SizeType mFanins[4];

};

END_NAMESPACE_DRUID_FSIM

#endif // SNGATE_H
