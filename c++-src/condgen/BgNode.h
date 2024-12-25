#ifndef BGNODE_H
#define BGNODE_H

/// @file BgNode.h
/// @brief BgNode のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_DRUID

//////////////////////////////////////////////////////////////////////
/// @class BgNode BgNode.h "BgNode.h"
/// @brief BooleanGraph のノードを表すクラス
///
/// BooleanGraph は BDD の構造を CNF に変換するために変形を加えてもの
/// である．
/// 通常のノードはBDDと同じく自分の変数番号と2つのコファクターを表す
/// 子供のノードを持つ．
/// この場合，変数を x として，2つのコファクターをそれぞれ f0, f1 と
/// するとこのノードが真になる条件を表す CNF 式は
/// (x + f0)(~x + f1)
/// となる．これが条件リテラル l が 1 の時だけ成り立つようにすると，
/// (~l + x + f0)(~l + ~x + f1)
/// となる．
///
/// 特別な扱いが必要となるのはどちらかのコファクターが 0 または 1 の
/// 定数に縮退している場合である．
///
/// 一方のコファクターが 0 に縮退している場合，そのノードが表している関数は
/// 子供のノードが表している関数と自分自身の変数（もしくはその否定）との
/// AND である．
/// 今，f0 が 0 だとするとCNF式は
/// (~l + x)(~l + f1)
/// となる．上の式の f0 に 0 を代入すると (~l + x)(~l + ~x + f1)
/// となるが，1項めで(~l + x)となっているので２項めの ~x が成り立たない
/// ことがわかるので削除できる．
///
/// 同様に一方のコファクターが 1 に縮退している場合，そのノードが表して
/// いる関数は子供のノードが表している関数と自分自身の変数（もしくはその否定）
/// との OR である．
/// いま，f1 が 1 だとするとCNF式は
/// (~l + x + f0)
/// となる．こちらは単純に f1 = 1 を代入して２項めを取り除けばよい．
///
/// f0 が 1 の場合や f1 が 0 の場合も x を ~x に置き換えるだけで同様の結果となる．
///
/// さらにこのようなノードが2つ以上つながっている場合はそれらをひとかたまりで
/// 考えたほうが式が簡単になる．
/// 例えば，AND型(片方が0)のノードが2つつながっている場合，その変数を x1, x2
/// として唯一のコファクターを f1 とすると CNF 式は
/// (~l + x1)(~l + x2)(~l + f1)
/// となる．
/// 同様に，OR型(片方が1)のノードが2つつながっている場合ｈ，その変数を x1, x2
/// として唯一のコファクターを f0 とすると CNF 式は
/// (~l + x1 + x2 + f0)
/// となる．
///
/// 最後の特別なケースは2つのコファクターが共に定数の場合で，これは単純に
/// リテラル関数を表している．
///
/// 以上から BooleanGraph のノードとして以下の5種類を考える．
/// - PLIT: 正のリテラルを表す(f0 = 0, f1 = 1)
/// - NLIT: 負のリテラルを表す(f0 = 1, f1 = 0)
/// - AND:  AND 条件を表す(f0 = 0 or f1 = 0)
/// - OR:   OR 条件を表す(f0 = 1 or f1 = 1)
/// - DEC:  通常の Shannon 分解(Boole 分解)を表す．
///
/// 実は DEC タイプも f0 と f1 の関係で3種類に分類することができる．
/// - DEC1: f0 < f1 の場合，
///   (~l + f1)(~l + x + f0)
///   と変形できる．[元の式に (~l + ~x + f1) を追加する．]
/// - DEC0: f0 > f0 の場合，
///   (~l + f0)(~l + ~x + f1)
///   と変形できる．[元の式に (~l + x + f0) を追加する．]
/// - DEC2: それ以外
///   (~l + x + f0)(~l + ~x + f1)
///
/// PLIT/NLIT ノードは変数番号のみを持つ．
/// AND/OR ノードは条件となっているリテラルノードのリストと定数でない
/// コファクターを表すノードを持つ．
/// DEC ノードは変数番号と2つのコファクターノードを持つ．
///
/// BgNode の各節点は必ずもとの BDD のいずれかの節点に対応する．
/// よって BgNode の節点数はもとの BDD のノード数のオーダーで
/// 抑えられる（ただし否定枝を展開する必要がある）．
///
/// タイプによって持つ情報が異なるので仮想関数で対応する．
/// このクラスはそのための純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class BgNode
{
public:

  /// @brief ノードの種類を表す列挙型
  enum Type {
    PLIT,
    NLIT,
    AND,
    OR,
    DEC0,
    DEC1,
    DEC2
  };


public:

  /// @brief PLIT タイプのノードを作るクラスメソッド
  static
  const BgNode*
  new_PLIT(
    SizeType varid ///< [in] 変数番号
  );

  /// @brief NLIT タイプのノードを作るクラスメソッド
  static
  const BgNode*
  new_NLIT(
    SizeType varid ///< [in] 変数番号
  );

  /// @brief AND タイプのノードを作るクラスメソッド
  static
  const BgNode*
  new_AND(
    const vector<const BgNode*>& operand_list, ///< [in] オペランドリスト
    const BgNode* cofactor                     ///< [in] コファクターノード
  );

  /// @brief OR タイプのノードを作るクラスメソッド
  static
  const BgNode*
  new_OR(
    const vector<const BgNode*>& operand_list, ///< [in] オペランドリスト
    const BgNode* cofactor                     ///< [in] コファクターノード
  );

  /// @brief DEC0 タイプのノードを作るクラスメソッド
  static
  const BgNode*
  new_DEC0(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  );

  /// @brief DEC0 タイプのノードを作るクラスメソッド
  static
  const BgNode*
  new_DEC1(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  );

  /// @brief DEC0 タイプのノードを作るクラスメソッド
  static
  const BgNode*
  new_DEC2(
    SizeType varid,          ///< [in] 変数番号
    const BgNode* cofactor0, ///< [in] 負のコファクター
    const BgNode* cofactor1  ///< [in] 正のコファクター
  );

  /// @brief デストラクタ
  virtual
  ~BgNode() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 種類を返す．
  virtual
  Type
  type() const = 0;

  /// @brief PLIT 型の時に true を返す．
  virtual
  bool
  is_PLIT() const;

  /// @brief NLIT 型の時に true を返す．
  virtual
  bool
  is_NLIT() const;

  /// @brief AND 型の時に true を返す．
  virtual
  bool
  is_AND() const;

  /// @brief OR 型の時に true を返す．
  virtual
  bool
  is_OR() const;

  /// @brief DEC0 型の時に true を返す．
  virtual
  bool
  is_DEC0() const;

  /// @brief DEC1 型の時に true を返す．
  virtual
  bool
  is_DEC1() const;

  /// @brief DEC2 型の時に true を返す．
  virtual
  bool
  is_DEC2() const;

  /// @brief 変数番号を返す．
  ///
  /// PLIT|NLIT|DEC0|DEC1|DEC2 の時のみ有効
  virtual
  SizeType
  varid() const;

  /// @brief オペランドの数を返す．
  ///
  /// AND|OR の時のみ有効
  virtual
  SizeType
  operand_num() const;

  /// @brief オペランドを返す．
  ///
  /// AND|OR の時のみ有効
  virtual
  const BgNode*
  operand(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < operand_num() )
  ) const;

  /// @brief オペランドのリストを返す．
  ///
  /// AND|OR の時のみ有効
  virtual
  vector<const BgNode*>
  operand_list() const;

  /// @brief コファクターノードを返す．
  ///
  /// AND|OR の時のみ有効
  virtual
  const BgNode*
  cofactor() const;

  /// @brief 負のコファクターを返す．
  ///
  /// DEC0|DEC1|DEC2 の時のみ有効
  virtual
  const BgNode*
  cofacotr0() const;

  /// @brief 正のコファクターを返す．
  ///
  /// DEC0|DEC1|DEC2 の時のみ有効
  virtual
  const BgNode*
  cofacotr1() const;

  /// @brief このノードを CNF に変換する際のサイズを返す．
  virtual
  CnfSize
  cnf_size() const = 0;

};

END_NAMESPACE_DRUID

#endif // BGNODE_H
