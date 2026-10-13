#ifndef BUILD_H
#define BUILD_H

#include <libultraship/libultra.h>
#ifdef __cplusplus
extern "C" {
#endif
// 2S2H [Port] Version information
extern char gBuildVersion[];
extern u16 gBuildVersionMajor;
extern u16 gBuildVersionMinor;
extern u16 gBuildVersionPatch;

extern char gGitBranch[];
extern char gGitCommitHash[];
extern char gGitCommitTag[];
extern char gBuildTeam[];
extern char gBuildDate[];
extern char gBuildMakeOption[];
#ifdef __cplusplus
}
#endif


#endif
