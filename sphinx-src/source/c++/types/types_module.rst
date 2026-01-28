基本的なデータ構造
===================

回路構造を表すクラス
~~~~~~~~~~~~~~~~~~~~~

回路全体は `TpgNetwork` で表す．
予め対象となる故障の種類を指定する必要がある．

縮退故障を対象として `blif` ファイルを読み込む場合には
以下のようなコードを用いる．

.. code-block:: c++

   network = TpgNetwork::read_blif(filename, FaultType::StuckAt)

もとの `blif` ファイルが D-FF を含む( `.latch` 文を含む)場合にも
D-FF の出力を擬似入力，D-FF の入力を擬似出力とみなした回路となる．

遷移故障を対象として `.bench` ( `iscas89` )ファイルを読み込む場合には
以下のようなコードを用いる．

.. code-block:: c++

   network = TpgNetwork::read_iscas89(filename, FaultType::TransitionDelay)

`blif` ファイルの `.names` 文および `.bench` ファイルのゲートは
`TpgGate` に対応する．
論理的には `TpgNetwork` は `TpgGate` を節点とした有向グラフで表される．
縮退故障および遷移故障は `TpgGate` の入力および出力に対して
設定される．

ただし，テスト生成や故障シミュレーションを効率よく行うためには
各々の節点が単純な論理ゲートであることが望ましい．
そこで， `BUF`, `NOT`, `AND`, `NAND`, `OR`, `NOR`, `XOR`, `XNOR`
の機能を持つ `TpgNode` を用意する．
もしも `TpgGate` の実現している機能が複雑な論理式で表されている
場合には複数の `TpgNode` に分解する．
そこで，実際には `TpgNetwork` は `TpgNode` の有向グラフとして
表されている．
故障の定義は `TpgGate` を対象に行われる．

互いに連結したファンアウト数が1の `TpgNode` で構成される
部分グラフを「ファンアウトフリー領域(Fanout Free Region: FFR)」
と呼ぶ．
FFR は `TpgFFR` で表される．

ある節点 u と v に対して，節点 u から外部出力へ至るいかなる経路も節点 v
を含む時，v は u の支配節点(dominator)であると言う．
節点 v を支配節点とするような節点 u の集合から誘導される部分グラフを
節点 v を根とした「ファンアウトフリーコーン(Fanout Free Cone: FFC)」
と呼ぶ，ファンアウトフリーコーンのなかで極大なものを
「極大ファンアウトフリーコーン(Maximal Fanout Free Cone: MFFC)」
と呼ぶ．
明らかに MFFC の根の節点は自分自身以外に支配節点を持たない．
MFFC は `TpgMFFC` で表される．
MFFC は１つもしくは複数の FFR から構成される．

`TpgNetwork` や `TpgNode` はすべて `TpgBase` の派生クラスとなっている．
`TpgBase` は内部で `NetworkRep` というクラスへの共有ポインタを持ってい
る．
この `NetworkRep` が実際の回路構造を持っており，
すべての `NetworkRep` への参照がなくなった時にそのオブジェクトは破棄さ
れる仕組みになっている．
これはネットワーク本体のオブジェクトが破棄されたあとで内部のノードに対
するポインタが参照されるバグを予防するためのものである．

そのため `TpgNode` や `TpgFault` はコピーや代入も実際には
内部の `NetworkRep` の共有ポインタをコピーするだけの
簡単な操作で実現できる．

そのため `TpgNode` のリストを表すためには `std::vector<TpgNode>`
で実装することも可能で問題なく動作するが，
例えば回路全体のノードのリストを取得しようと思った場合，
ノードごとに共有ポインタのコピーが発生するのはあまり効率的ではない．
そこで `TpgNodeList` では `NetworkRep` の共有ポインタを1つだけ持ち，
残りはノード番号のリストを保持することで意味的には
`std::vector<TpgNode>` と同等の操作を行えるようにしている．
`TpgXXXList` はすべて同様の実装方法で作られている．

.. toctree::
   :maxdepth: 1

   tpgbase
   tpgnetwork
   tpgnode
   tpgnodelist
   tpggate
   tpggatelist
   tpgffr
   tpgffrlist
   tpgmffc
   tpgmffclist


故障を表すクラス
~~~~~~~~~~~~~~~~~

druid では縮退故障と遷移故障をあつかう．
ゲート遷移故障も対応可能となっている．
故障の種類は `FaultType` で表す．
縮退故障および遷移故障の場合は故障箇所
と故障値を定める必要がある．

故障箇所は `TpgGate` の出力もしくは入力
である．出力の場合，「ステム(Stem)」の故障，
入力の場合，「ブランチ(Branch)」の故障となる．
ブランチの故障の場合には `TpgGate` だけではなく
入力番号も必要となる．

縮退故障の故障値は 0 か 1 なのでそれぞれ
`Fval2::zero` および `Fval2::one` で表す．
遷移故障も同様に表すが，立ち上がり遅延と
立ち下がり遅延を 0 と 1 で区別するのは
わかりにくいため関数 `is_rise_transition()`
および `is_fall_transition()` を用意している．

.. toctree::
   :maxdepth: 1

   fval2
   faulttype
   faultstatus
   tpgfault
   tpgfaultlist

値の割り当てを表すクラス
~~~~~~~~~~~~~~~~~~~~~~~~~~

テストパタンや部分的な値の割り当てを表すために
いくつかのデータ構造を用意している．

まず，単独の `TpgNode` (の出力)に対する値割り当てを表すクラスとして
`Assign` を用いる．
単純には `TpgNode` と値(0 か 1)の組だが，
遷移故障を扱う場合には前の時刻の値と現在の時刻の値の2種類が
あるので `TpgNode` ，時刻(0 か 1)，値(0 か 1) の3つ組
で表される．
`AssignList` は `Assign` のリストである．

ワード(現代のアーキテクチャでは64ビット)内に
ビットを詰め込むために `PackedVal` という型を定義している．
これは c++ の標準ライブラリの `std::uint64_t` の別名である．
`PackedVal` を2つ使って3値のベクタを表すクラスが
`PackedVal3` である．
通常の整数型と同様のビットワイズ論理演算をサポートしている．

`PackedVal3` を複数用いてテストベクタ全体を表すクラスが
`BitVector` である．
ただし，関数のシグネチャで型を区別するために
`InputVector` ， `DffVector` ， `TestVector`
という異なった継承クラスを定義している．
中身は全て同じである．

.. toctree::
   :maxdepth: 1

   assign
   assignlist
   packedval
   packedval3
   bitvector
   dffvector
   inputvector
   testvector
