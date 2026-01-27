#ifndef FAULT_STEM_H
#define FAULT_STEM_H

/// @file Fault_Stem.h
/// @brief Fault_Stem とその継承クラスのヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "FaultRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Fault_Stem Fault_Stem.h "Fault_Stem.h"
/// @brief ステムの故障を表すクラス
//////////////////////////////////////////////////////////////////////
class Fault_Stem :
  public FaultRep
{
public:

  /// @brief コンストラクタ
  Fault_Stem(
    SizeType id,        ///< [in] ID番号
    const GateRep* gate ///< [in] 対象のゲート
  ) : FaultRep{id, gate}
  {
  }

  /// @brief デストラクタ
  ~Fault_Stem() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障伝搬の起点となるノードを返す．
  const NodeRep*
  origin_node() const override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の内容を表す文字列の基本部分を返す．
  std::string
  str_base() const;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_StemSa Fault_Stem.h "Fault_Stem.h"
/// @brief ステムの縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class Fault_StemSa :
  public Fault_Stem
{
public:

  /// @brief コンnストラクタ
  Fault_StemSa(
    SizeType id,        ///< [in] ID番号
    const GateRep* gate ///< [in] 対象のゲート
  ) : Fault_Stem{id, gate}
  {
  }

  /// @brief デストラクタ
  ~Fault_StemSa() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_StemSa0 Fault_Stem.h "Fault_Stem.h"
/// @brief ステムの0縮退故障
//////////////////////////////////////////////////////////////////////
class Fault_StemSa0 :
  public Fault_StemSa
{
public:

  /// @brief コンストラクタ
  Fault_StemSa0(
    SizeType id,        ///< [in] ID番号
    const GateRep* gate ///< [in] 対象のゲート
  ) : Fault_StemSa{id, gate}
  {
  }

  /// @brief デストラクタ
  ~Fault_StemSa0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_StemSa1 Fault_Stem.h "Fault_Stem.h"
/// @brief ステムの1縮退故障
//////////////////////////////////////////////////////////////////////
class Fault_StemSa1 :
  public Fault_StemSa
{
public:

  /// @brief コンストラクタ
  Fault_StemSa1(
    SizeType id,        ///< [in] ID番号
    const GateRep* gate ///< [in] 対象のゲート
  ) : Fault_StemSa{id, gate}
  {
  }

  /// @brief デストラクタ
  ~Fault_StemSa1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_StemTd Fault_Stem.h "Fault_Stem.h"
/// @brief 出力の縮退故障を表すクラス
//////////////////////////////////////////////////////////////////////
class Fault_StemTd :
  public Fault_Stem
{
public:

  /// @brief コンストラクタ
  Fault_StemTd(
    SizeType id,        ///< [in] ID番号
    const GateRep* gate ///< [in] 対象のゲート
  ) : Fault_Stem{id, gate}
  {
  }

  /// @brief デストラクタ
  ~Fault_StemTd() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障の種類を返す．
  FaultType
  fault_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_StemRise Fault_Stem.h "Fault_Stem.h"
/// @brief ステムの rise 遷移故障
//////////////////////////////////////////////////////////////////////
class Fault_StemRise :
  public Fault_StemTd
{
public:

  /// @brief コンストラクタ
  Fault_StemRise(
    SizeType id,        ///< [in] ID番号
    const GateRep* gate ///< [in] 対象のゲート
  ) : Fault_StemTd{id, gate}
  {
  }

  /// @brief デストラクタ
  ~Fault_StemRise() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Fault_StemFall Fault_Stem.h "Fault_Stem.h"
/// @brief ステムの fall 遷移故障
//////////////////////////////////////////////////////////////////////
class Fault_StemFall :
  public Fault_StemTd
{
public:

  /// @brief コンストラクタ
  Fault_StemFall(
    SizeType id,        ///< [in] ID番号
    const GateRep* gate ///< [in] 対象のゲート
  ) : Fault_StemTd{id, gate}
  {
  }

  /// @brief デストラクタ
  ~Fault_StemFall() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 故障値を得る．
  Fval2
  fval() const override;

  /// @brief 故障が励起して origin_node の出力まで伝搬する条件を求める．
  std::vector<SizeType>
  excitation_condition() const override;

  /// @brief 故障の内容を表す文字列を返す．
  std::string
  str() const override;

};

END_NAMESPACE_DRUID

#endif // FAULT_STEM_H
