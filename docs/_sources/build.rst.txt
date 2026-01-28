ビルドとインストール
=====================

必要な外部ソフトウェア
-----------------------

Druid のビルドには以下のソフトウェアが必要となる．

- cmake

  3.14 以上を仮定

- ninja

  なければ make で代用も可

- python3

  - pytest もテストで使用する．

- doxygen

  APIドキュメントの自動生成に使用


Druid の取得
-------------

.. code-block:: sh

   $ git clone --recurse-submodules https:://github.com/yusuke-matsunaga/druid.git

内部で ymworks.git というサブモジュールを使用しているので
`--recurse-submodules`
オプションが必要となる．
もしもこのオプションを付け忘れて druid.git を取得した場合は
展開されたソースディレクトリに行って以下のコマンドを実行すればよい．

.. code-block:: sh

   $ cd <druidのソースディレクトリ>
   $ git submodule init
   $ git submodule update

ビルド環境の準備1
-------------------

druid のソースディレクトリを `DRUID_SRC` とする．
このディレクトリは git で管理されているのでここで
コンパイル作業を行うことはあまり好ましくない．
そこで別のビルド用ディレクトリを用意する．
ここでそのビルド用ディレクトリを `DRUID_BUILD` とする．
このディレクトリは今の時点では存在していなくてもよい．
以下のコマンドでビルド用ディレクトリが作られる．

.. code-block:: sh

   $ cd $DRUID_SRC
   $ ./mk_builddir.sh $DRUID_BUILD

すると以下のようなプロンプトが表示される．

.. code-block:: sh

   ***
   source  directory: <DRUID_SRC>
   build   directory: <DRUID_BUILD>
   ***
   continue ? (yes/no):_

この内容で問題がなければ `yes` とタイプしてエンターキーを押す．
やり直す場合には `no` と入力する．
なお，MacOS の場合にはシェルの `echo -n` で改行無効の動作とならない
ため表示が少しことなる．

ビルド環境の準備2
------------------

次に `DRUID_BUILD` ディレクトリに移動して以下のコマンドを実行する．

.. code-block:: sh

   $ cd $DRUID_BUILD
   $ ./boot.sh
   .
   .
   .
   .
   -- Configuring done (*.*s)
   -- Generating done (*.*s)
   -- Build files have been wrtten to: *****
   set up completed, then type 'ninja' to make all staffs.

ninja がインストールされていない場合には最後の行の `ninja` は `make`
になっている．
通常はこの後 ninja か make を実行することでビルド作業が開始される．

もしも必要なパッケージがインストールされていない場合や，cmake が
それを見つけられなかった場合にはなんらかのエラーメッセージが出力
される．

`boot.sh` はその名前が示すとおりシェルスクリプトであり，中身は
適切な設定を行って cmake を起動するだけのものである．
なので cmake の使用法がわかっている場合には直接 cmake を実行してもよい．
このスクリプトを用意した理由は特に開発時に同じ設定を毎回手で打ち込む
ことを回避するためのものである．

ただし， `boot.sh` は上記の `mk_builddir.sh` で自動的にコピーされるた
め， `boot.sh` の内容を書き換えてもその後に `mk_builddir.sh` を再実行
してしまうと上書きされてしまう．
そのため， `boot.sh` は内部で `boot-opt.sh` という名前のファイルをイン
クルードしている．
このファイルも `mk_builddir.sh` で自動的にコピーされるが，
すでに同名のファイルが存在している場合には上書きするか既存のファイルを
残すかの選択ができるようになっている．
以下は `mk_builddir.sh` で指定したビルドディレクトリに既に
`boot-opt.sh` ファイルが存在したときの出力である．

.. code-block:: sh

   Try to copy boot-opt.sh to <DRUID_BUILD>
   <DRUID_BUILD>/boot-opt.sh already exists.
   Would you overwrite if? (yes/no):_

表示の通り `yes` で上書きを行う．

初期状態の `boot-opt.sh` はコメントのみのファイルである

.. code-block:: sh

   # @file  boot-opt.sh
   # @brief boot.sh に追加で設定する内容を記述するファイル
   # @author Yusuke Matsunaga (松永 裕介)
   #
   # Copyright (C) 2016, 2017 Yusuke Matsunaga (松永 裕介)
   # All rights reserved

   # オプション文字列を追加する場合は以下のように
   # 既存のoptions変数に追加すること．
   # options="$options -DXXX=YYY"

   # 環境変数の設定は以下のように行う．
   # export PATH="/usr/local/bin:$PATH"

このファイルはただのシェルスクリプトなので自由に変更可能だが，
主な使い方は以下の2つである．

- シェル変数 `$options` の設定
- 実行パス `$PATH` の設定

`$options` は `boot.sh` 中で cmake を起動する時にそのままオプションと
して渡される．
そこで cmake に渡すオプションをここで記述する．
最も一般的なオプションは cmake の CMakeLists.txt 中の変数の値を
設定するもので， `-DXXX=YYY` の形で cmake 変数 `XXX` の値を
`YYY` に設定する．

実行パスの指定は外部ソフトウェアを標準的な場所以外にインストールした
場合に必要となる．
たとえば MacOS で homebrew を使ってインストールされたパッケージは
`/usr/bin` や `/usr/local/bin` にはインストールされない．
そこでそれらの実行ファイルを cmake が見つけられるように実行パス
の設定を行う．
場合によってはライブラリの検索パスの設定が必要となることもある．


インストール
~~~~~~~~~~~~~

ビルドが完了したあとで以下のコマンドを実行するとビルド結果がインストー
ルされる．

.. code-block:: sh

   $ ninja install

とはいっても昨今はライブラリなどのファイルを標準的な場所にインストール
するのはバージョン管理などの面で不都合が多いのでここでは
`DRUID_BUILD/stage`
というディレクトリを作成し，その下に必要なファイルをコピーするようにし
ている．
現時点では druid の python 拡張モジュール `lib/druid.so` と druid モ
ジュールを組み込んだ python インタプリタ `bin/druid` と `bin/druid_d`
がインストールされる．

`bin/druid` は `lib/druid.so` と同等のライブラリを既にスタティックリン
クした python インタプリタで， `$PYTHONPATH` に `lib/druid.so` の場所
をついかした通常の python とほぼ同様のものとなる．

`bin/druid_d` はデバッグオプションをオンにしてコンパイルしたもので
デバッガを用いて起動するとシンボル情報を取得することができる．
