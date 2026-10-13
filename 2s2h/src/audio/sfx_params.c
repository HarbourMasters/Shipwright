#include "global.h"

#define DEFINE_SFX(_0, importance, distParam, randParam, flags2, flags1)           \
    { importance, flags2,                                                          \
      ((((distParam) << SFX_PARAM_DIST_RANGE_SHIFT) & SFX_PARAM_DIST_RANGE_MASK) | \
       (((randParam) << SFX_PARAM_RAND_FREQ_RAISE_SHIFT) & SFX_PARAM_RAND_FREQ_RAISE_MASK) | (flags1)) },

SfxParams sEnemyBankParams[] = {
#include "tables/sfx/enemybank_table.h"
};

SfxParams sPlayerBankParams[] = {
#include "tables/sfx/playerbank_table.h"
};

SfxParams sItemBankParams[] = {
#include "tables/sfx/itembank_table.h"
};

SfxParams sEnvBankParams[] = {
#include "tables/sfx/environmentbank_table.h"
};

SfxParams sSystemBankParams[] = {
#include "tables/sfx/systembank_table.h"
};

SfxParams sOcarinaBankParams[] = {
#include "tables/sfx/ocarinabank_table.h"
};

SfxParams sVoiceBankParams[] = {
#include "tables/sfx/voicebank_table.h"
};

#undef DEFINE_SFX

SfxParams* gSfxParams[7] = {
    sPlayerBankParams, sItemBankParams,    sEnvBankParams,   sEnemyBankParams,
    sSystemBankParams, sOcarinaBankParams, sVoiceBankParams,
};
