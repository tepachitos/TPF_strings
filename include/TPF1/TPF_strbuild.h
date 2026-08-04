//
// Created by Tepachitos on 3/22/26.
//

#ifndef TPF_STRBUILD_H
#define TPF_STRBUILD_H

#include "TPF_strview.h"

typedef struct TPF_SB TPF_SB;

TPF_SB *TPF_CreateSB(void);
void TPF_DestroySB(TPF_SB *sb);
bool TPF_SBReserve(TPF_SB *sb, size_t req);
size_t TPF_SBTotalLen(const TPF_SB *sb);
bool TPF_SBAppend(TPF_SB *sb, TPF_SV sv);
bool TPF_SBAppendf(TPF_SB *sb, const char *fmt, ...);
bool TPF_SBAppendRepeated(TPF_SB *sb, TPF_SV sv, Uint32 times);
bool TPF_SBAppendLowered(TPF_SB *sb, TPF_SV sv);
bool TPF_SBAppendUppered(TPF_SB *sb, TPF_SV sv);
bool TPF_SBAppendReplaced(TPF_SB *sb, TPF_SV ori_sv, TPF_SV old_sv,
                          TPF_SV new_sv, size_t n);
bool TPF_SBAppendReplacedAll(TPF_SB *sb, TPF_SV ori_sv, TPF_SV old_sv,
                             TPF_SV new_sv);
bool TPF_SBAppendJoined(TPF_SB *sb, size_t sv_count, const TPF_SV *sv,
                        TPF_SV sep);

TPF_SV TPF_SBBuildSV(const TPF_SB *sb);
char *TPF_SBBuildCStr(const TPF_SB *sb);

#define TPF_STRBUILD_STRING_ALIGN 8
#endif // TPF_STRBUILD_H
