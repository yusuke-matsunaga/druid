
/// @file py_druid.cc
/// @brief py_druid の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "druid.h"
#ifdef HAS_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif


BEGIN_NAMESPACE_DRUID

extern "C"
PyObject* PyInit_druid();

bool
get_line(
  const string& prompt,
  string& linebuf
)
{
#ifdef HAS_READLINE
  auto line_read = readline(prompt.c_str());
  if ( line_read == nullptr ) {
    // EOF
    return false;
  }
  if ( line_read[0] ) {
    add_history(line_read);
  }
  linebuf = string{line_read};
  free(line_read);
  return true;
#else
  cerr << prompt;
  cerr.flush();
  if ( getline(cin, linebuf) ) {
    return true;
  }
  else {
    return false;
  }
#endif
}

// @brief インタプリタのメイン処理を行う．
int
main_loop(
  int argc,   ///< [in] コマンドラインの引数の数
  char** argv ///< [in] コマンドラインの引数の配列
)
{
  if ( argc == 1 ) {
    // インタラクティブモード
    const char* prompt{"% "};
    for ( ; ; ) {
      string linebuf;
      if ( !get_line(prompt, linebuf) ) {
	break;
      }
      int err = PyRun_SimpleString(linebuf.c_str());
      if ( err ) {
	cerr << "Error in executing: " << linebuf << endl;
      }
    }
  }
  else if ( argc >= 2 ) {
#if 0
    // 引数を lua のグローバル変数 "arg" にセットする．
    create_table(argc, 0);
    for ( int i = 0; i < argc; ++ i ) {
      push_string(argv[i]);
      set_table(-2, i);
    }
    set_global("arg");
    // 先頭の引数をスクリプトファイルとみなして実行する．
    char* init_file = argv[1];
    string script{argv[1]};
    // スクリプトファイルの場所を変数 "script_dir" にセットする．
    char* dir_name = dirname(init_file);
    push_string(dir_name);
    set_global("script_dir");
    if ( script != string{} && L_dofile(script.c_str()) ) {
      cerr << to_string(-1) << endl;
      return 1;
    }
#endif
  }
  return 0;
}

END_NAMESPACE_DRUID

int
main(
  int argc,
  char** argv
)
{
  using namespace std;

  // お約束
  auto *program = Py_DecodeLocale(argv[0], NULL);
  if ( program == nullptr ) {
    cerr << "Fatal error: cannot decode argv[0]" << endl;
    exit(1);
  }
  Py_SetProgramName(program);

  // Python インタプリタの初期化
  PyImport_AppendInittab("druid", &DRUID_NAMESPACE::PyInit_druid);
  Py_Initialize();

  DRUID_NAMESPACE::main_loop(argc, argv);

  // Python インタプリタの終了処理
  if ( Py_FinalizeEx() < 0 ) {
    // Python のお約束
    return 120;
  }
  PyMem_RawFree(program);
  return 0;
}
