#ifndef NODE_LOGIC_H
#define NODE_LOGIC_H

/// @file Node_Logic.h
/// @brief Node_Logic のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "NodeRep.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class Node_Logic Node_Logic.h "Node_Logic.h"
/// @brief 論理ノードを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_Logic :
  public NodeRep
{
protected:
  //////////////////////////////////////////////////////////////////////
  // コンストラクタ/デストラクタ
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  Node_Logic(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインリスト
  ) : NodeRep(id, fanin_list)
  {
  }

  /// @brief デストラクタ
  ~Node_Logic() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 構造情報を得る関数
  //////////////////////////////////////////////////////////////////////

  /// @brief logic タイプの時 true を返す．
  bool
  is_logic() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_C0 Node_C0.h "Node_C0.h"
/// @brief constant-0 を表すクラス
//////////////////////////////////////////////////////////////////////
class Node_C0 :
  public Node_Logic
{
public:

  /// @brief コンストラクタ
  Node_C0(
    SizeType id ///< [in] ノード番号
  ) : Node_Logic(id, {})
  {
  }

  /// @brief デストラクタ
  ~Node_C0() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_C1 Node_C1.h "Node_C1.h"
/// @brief constant-1 を表すクラス
//////////////////////////////////////////////////////////////////////
class Node_C1 :
  public Node_Logic
{
public:

  /// @brief コンストラクタ
  Node_C1(
    SizeType id ///< [in] ノード番号
  ) : Node_Logic(id, {})
  {
  }

  /// @brief デストラクタ
  ~Node_C1() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_BUFF Node_BUFF.h "Node_BUFF.h"
/// @brief buffer を表すクラス
//////////////////////////////////////////////////////////////////////
class Node_BUFF :
  public Node_Logic
{
public:

  /// @brief コンストラクタ
  Node_BUFF(
    SizeType id,         ///< [in] ノード番号
    const NodeRep* fanin /// @param[in] ファンイン
  ) : Node_Logic(id, {fanin})
  {
  }

  /// @brief デストラクタ
  ~Node_BUFF() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  cval() const override;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  nval() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_NOT Node_NOT.h "Node_NOT.h"
/// @brief inverter を表すクラス
//////////////////////////////////////////////////////////////////////
class Node_NOT :
  public Node_Logic
{
public:

  /// @brief コンストラクタ
  Node_NOT(
    SizeType id,         ///< [in] ノード番号
    const NodeRep* fanin ///< [in] ファンイン
  ) : Node_Logic(id, {fanin})
  {
  }

  /// @brief デストラクタ
  ~Node_NOT() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  cval() const override;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  nval() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_AND Node_AND.h "Node_AND.h"
/// @brief ANDゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_AND :
  public Node_Logic
{
public:

  /// @brief コンストラクタ
  Node_AND(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  ) : Node_Logic(id, fanin_list)
  {
  }

  /// @brief デストラクタ
  ~Node_AND() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  cval() const override;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  nval() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_NAND Node_NAND.h "Node_NAND.h"
/// @brief NANDゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_NAND :
  public Node_AND
{
public:

  /// @brief コンストラクタ
  Node_NAND(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  ) : Node_AND(id, fanin_list)
  {
  }

  /// @brief デストラクタ
  ~Node_NAND() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_OR Node_OR.h "Node_OR.h"
/// @brief ORゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_OR :
  public Node_Logic
{
public:

  /// @brief コンストラクタ
  Node_OR(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  ) : Node_Logic(id, fanin_list)
  {
  }

  /// @brief デストラクタ
  ~Node_OR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  cval() const override;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  nval() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_NOR Node_NOR.h "Node_NOR.h"
/// @brief NORゲートを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_NOR :
  public Node_OR
{
public:

  /// @brief コンストラクタ
  Node_NOR(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  ) : Node_OR(id, fanin_list)
  {
  }

  /// @brief デストラクタ
  ~Node_NOR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

};


//////////////////////////////////////////////////////////////////////
/// @class Node_XOR Node_XOR.h "Node_XOR.h"
/// @brief XORを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_XOR :
  public Node_Logic
{
public:

  /// @brief コンストラクタ
  Node_XOR(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  ) : Node_Logic(id, fanin_list)
  {
  }

  /// @brief デストラクタ
  ~Node_XOR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

  /// @brief controling value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  cval() const override;

  /// @brief noncontroling valueを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  nval() const override;

  /// @brief controling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  coval() const override;

  /// @brief noncontroling output value を得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  /// ない場合は Val3::_X を返す．
  Val3
  noval() const override;

};

//////////////////////////////////////////////////////////////////////
/// @class Node_XNOR Node_XNOR.h "Node_XNOR.h"
/// @brief XNORを表すクラス
//////////////////////////////////////////////////////////////////////
class Node_XNOR :
  public Node_XOR
{
public:

  /// @brief コンストラクタ
  Node_XNOR(
    SizeType id,                                  ///< [in] ノード番号
    const std::vector<const NodeRep*>& fanin_list ///< [in] ファンインのリスト
  ) : Node_XOR(id, fanin_list)
  {
  }

  /// @brief デストラクタ
  ~Node_XNOR() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ゲートタイプを得る．
  ///
  /// is_logic() が false の場合の返り値は不定
  PrimType
  gate_type() const override;

};

END_NAMESPACE_DRUID

#endif // NODE_LOGIC_H
