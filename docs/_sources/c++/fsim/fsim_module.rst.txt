故障シミュレーター
====================

故障シミュレーターの初期化
~~~~~~~~~~~~~~~~~~~~~~~~~~~

論理的には故障シミュレーションは与えられた回路とテストパタンに対して
検出される故障を求めるものなので内部に状態を持つクラスにする必要はない
が，
効率的な処理を行うために回路の構造と対象の故障の情報を内部に保持して
おいたほうが都合がよい．
そのためのクラスが `Fsim` である．
`Fsim` は以下の形式で初期化する．

.. code-block:: c++

   // Fsim のコンストラクタ
   Fsim(
     const TpgNetwork& network,
     const TpgFaultList& fault_list,
     const JsonValue& option = {}
   );

`fault_list` に含まれる故障はすべて `network`
の故障でなければならない．

故障シミュレーターには以下のオプションを指定することができる．
オプションは `JsonValue` の形で指定する．

============= ======== ======================================== =============
キーワード    タイプ   説明                                     デフォルト値
============= ======== ======================================== =============
has_x         bool     true の時に3値のシミュレーションを行う． false
multi_thread  bool     true の時にマルチスレッド実行を行う．    false
============= ======== ======================================== =============

さらに `TpgNetwork` の故障の種類に応じて1時刻分のみを持つ単純な組み合
わせ回路用の故障シミュレーターか1時刻前の値も持つブロードサイド方式
用の故障シミュレーターが自動的に選択される．


故障のスキップマークに関する操作
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

故障シミュレーターは基本的にコンストラクタで与えられた故障を対象とする．
ただし，故障ドロップの動作を行うために内部で故障のスキップマークを
管理している．
スキップマークが `true` の故障はシミュレーションの対象から除外される．
初期状態では全ての故障のスキップマークは `false` となっている．

スキップマークは以下のメンバ関数で操作される．

.. code-block:: c++

   // すべての故障にスキップマークをつける．
   void
   set_skip_all();

   // fault にスキップマークをつける．
   void
   set_skip(
     const TpgFault& fault
   );

   // fault_list に含まれる故障にスキップマークをつける．
   void
   set_skip(
     const TpgFaultList& fault_list
   );

   // すべての故障のスキップマークを消す．
   void
   clear_skip_all();

   // fault のスキップマークを消す．
   void
   clear_skip(
     const TpgFault& fault
   );

   // fault_list に含まれる故障のスキップマークを消す．
   void
   clear_skip(
     const TpgFaultList& fault_list
   );

   // fault のスキップマークを返す．
   bool
   get_skip(
     const TpgFault& fault
   );


単一故障単一パタンの故障シミュレーションを行う．
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

入力として `TestVector` を用いる関数 `spsfp()` と `AssignList` を用いる関数
`spsfp()` および `xspsfp()` が用意されている．
`AssignList` を用いる `spsfp()` の場合はすべて外部入力ノードに対する割り当て
になっていなければならない．
`xspsfp()` の場合は回路内部のノードに対する割り当てでもシミュレーショ
ンをおこなうことができる．
これらの関数は対象となる故障を指定する．
スキップマークの有無は無視される．

故障シミュレーションの結果として故障の検出が行えた場合には
返り値として `true` が返される．
検出出来ない場合には `false` が返される．
さらに各外部出力ごとの故障伝搬の結果を `DiffBits& dbits`
に格納する．

.. code-block:: c++

   bool
   spsfp(
     const TestVector& tv,
     const TpgFault& fault,
     DiffBits& dbits
   );

   bool
   spsfp(
     const AssignList& assign_list,
     const TpgFault& fault,
     DiffBits& dbits
   );

   bool
   xspsfp(
     const AssignList& assign_list,
     const TpgFault& fault,
     DiffBits& dbits
   );


故障並列シミュレーションを行う．
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

内部に設定されているスキップマークを参考にして
故障並列シミュレーションを行う．
とはいっても純粋な並列処理はできないので
故障が検出されるたびにコールバック関数が
呼び出される．
コールバック関数の型宣言は以下の通り．

.. code-block:: c++

   using cbtype1 = std::function<void(const TpgFault& f,
		                      const DiffBits& dbits)>;

`f` には検出された故障を，
`dbits` には各出力ごとの検出状態が与えられる．

入力パタンとして `TestVector` を用いる関数 `sppfp()`
と `AssignList` を用いる関数 `sppfp()`
および `xsppfp()` が用意されている．
これらの区別は `spsfp()` と同様である．

.. code-block:: c++

   void
   sppfp(
     const TestVector& tv,
     cbtype1 callback
   );

   void
   sppfp(
     const AssignList& assign_list,
     cbtype1 callback
   );

   void
   xsppfp(
     const AssignList& assign_list,
     cbtype1 callback
   );


パタン並列シミュレーションを行う．
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

内部に設定されているスキップマークを参考にして
パタン並列シミュレーションを行う．
とはいっても純粋な並列処理はできないので
故障が検出されるたびにコールバック関数が
呼び出される．
コールバック関数の型宣言は以下の通り．

.. code-block:: c++

   using cbtype2 = std::function<void(const TpgFault& f,
		                      const DiffBitsArray& dbits_array)>;

`f` には検出された故障が，
`dbits_array` には各出力ごとの検出状態が与えられる．
`DiffBitsArray` は意味的には `DiffBits` を64個集めたものである．
実際の実装では非ゼロの要素だけを保持している．

.. code-block:: c++

   void
   ppsfp(
     const std::vector<TestVector>& tv_list,
     cbtype2 callback
   );

`tv_list` にはシミュレーションを行うパタンのリストを指定する．
`tv_list` の要素数の最大値は `PV_BITLEN` (=64) である．



.. toctree::
   :maxdepth: 1

   fsim
   diffbits
   diffbitsarray
