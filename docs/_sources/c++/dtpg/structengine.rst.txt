StructEngine
==============

`StructEngine` は内部に SAT ソルバを持つ．
基本的にはコンストラクタで与えられた `network` 内のノードの入出力
関係を表すCNFをそのSATソルバに追加することになるが，
無駄なCNFを追加する必要はないのでどのノードを使用するかを指定する必要がある．

.. code-block:: c++

   void
   add_cur_node(
     const TpgNode& node
   );

   void
   add_cur_node_list(
     const TpgNodeList& node_list
   );

   void
   add_prev_node(
     const TpgNode& node
   );

   void
   add_prev_node_list(
     const TpgNodeList& node_list
   );

`add_cur_node()` と `add_cur_node_list()`
では指定されたノードの推移的ファンアウトの推移的ファンインを
対象のノードに追加する．
もちろん重複した追加は無視される．

`add_prev_node()` と `add_prev_node_list()`
では指定されたノードの推移的ファンインを1時刻前のノードの
対象に追加する．
もちろん重複した追加は無視される．
こちらは遷移故障の1時刻前の条件に用いられる．

これらの関数は処理を行うべきノードのリストに追加しているだけで
実際の処理は行わない．
未処理のノードは SATソルバを起動する時には処理されるようになっているが，
明示的に処理済みにするためには以下の関数を呼び出す．

.. code-block:: c++

   void
   update();

.. doxygenclass:: StructEngine
   :members:
