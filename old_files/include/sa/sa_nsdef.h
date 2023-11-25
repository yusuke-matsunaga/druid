#ifndef DRUID_SA_H
#define DRUID_SA_H

/// @file sa_nsdef.h
/// @brief Stuck-At テストの DRUID 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "druid.h"


// 名前空間の定義ファイル
// 必ず先頭でインクルードしなければならない．

//////////////////////////////////////////////////////////////////////
// 名前空間の定義用マクロ
// namespace でネストするのがいやなので define マクロでごまかす．
//////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_DRUID_SA \
BEGIN_NAMESPACE_DRUID \
BEGIN_NAMESPACE(nsSa)

#define END_NAMESPACE_DRUID_SA \
END_NAMESPACE(nsSa) \
END_NAMESPACE_DRUID


BEGIN_NAMESPACE_DRUID_SA

//////////////////////////////////////////////////////////////////////
// クラス名の宣言
//////////////////////////////////////////////////////////////////////

class FoCone;
class StructEnc;
class Rtpg;
class RtpgStats;
class MinPat;
class MinPatStats;

END_NAMESPACE_DRUID_SA

#endif // DRUID_SA_H
