#ifndef DRUID_TD_H
#define DRUID_TD_H

/// @file td_nsdef.h
/// @brief transition delay テストの DRUID 用の名前空間の定義
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

#define BEGIN_NAMESPACE_DRUID_TD \
BEGIN_NAMESPACE_DRUID \
BEGIN_NAMESPACE(nsTd)

#define END_NAMESPACE_DRUID_TD \
END_NAMESPACE(nsTd) \
END_NAMESPACE_DRUID


BEGIN_NAMESPACE_DRUID_TD

//////////////////////////////////////////////////////////////////////
// クラス名の宣言
//////////////////////////////////////////////////////////////////////

class FoCone;
class MffcCone;
class StructEnc;
class Rtpg;
class RtpgStats;
class MinPat;
class MinPatStats;

END_NAMESPACE_DRUID_TD

#endif // DRUID_TD_H
