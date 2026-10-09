#include <libultraship/bridge/consolevariablebridge.h>
#include "2s2h/GameInteractor/GameInteractor.h"
#include "2s2h/ShipInit.hpp"

extern "C" {
#include "variables.h"
#include "functions.h"
}

/*
 * The cutscene for healing Mikau has a CS_CMD_STOP_SEQ command for NA_BGM_ZORA_HALL for frames 0 and 1. On N64, the
 * BGM sequence is not yet loaded until frame 10, so there is no sequence to stop. The result is that the Great Bay
 * Coast sequence plays as normal. 2ship short circuits the delayed sequence loading, which means the main BGM sequence
 * is active by frame 0, which allows the CS_CMD_STOP_SEQ command to kill the BGM.
 *
 * Given that:
 * 1. The CS_CMD_STOP_SEQ call is for NA_BGM_ZORA_HALL, which wouldn't be playing in this scene anyway
 * 2. The CS_CMD_STOP_SEQ is immediately at the start for frames 0 and 1, while any other uses in the game have larger
 *    windows and occur later in their respective cutscenes
 * 3. CS_CMD_STOP_SEQ takes an argument for the specific sequence ID, but kills that type of sequence (main, sub, etc.)
 * 4. The subsequent cutscene of Link bowing at Mikau's grave fades out the BGM and then fades it back in
 * 5. This CS_CMD_STOP_SEQ call effectively does nothing in vanilla because of the delayed audio loading
 * 6. MM3D plays the Great Bay Coast BGM in this cutscene
 *
 * It is probably safe to say that this command is not supposed to be there. This ad-hoc enhancement exists to restore
 * the vanilla behavior.
 */

static HOOK_ID playGreatBayCoastBgmHookId = 0;

void RegisterHealingMikauAudioFix() {
    COND_ID_HOOK(OnActorInit, ACTOR_EN_ZOG, true, [](Actor* actor) {
        // This is the healing Mikau cutscene
        if (gSaveContext.save.entrance == ENTRANCE(GREAT_BAY_COAST, 9) && !playGreatBayCoastBgmHookId) {
            // The main BGM sequence is playing (no BGM plays at night)
            if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) != NA_BGM_DISABLED) {
                playGreatBayCoastBgmHookId =
                    GameInteractor::Instance->RegisterGameHookForID<GameInteractor::OnActorUpdate>(
                        ACTOR_EN_ZOG, [](Actor* actor) {
                            // If BGM is killed, replay it and kill this update hook
                            if (AudioSeq_GetActiveSeqId(SEQ_PLAYER_BGM_MAIN) == NA_BGM_DISABLED) {
                                SEQCMD_PLAY_SEQUENCE(SEQ_PLAYER_BGM_MAIN, 0, NA_BGM_GREAT_BAY_REGION);
                                if (playGreatBayCoastBgmHookId) {
                                    GameInteractor::Instance->UnregisterGameHookForID<GameInteractor::OnActorUpdate>(
                                        playGreatBayCoastBgmHookId);
                                    playGreatBayCoastBgmHookId = 0;
                                }
                            }
                        });
            }
        }
    });
}

static RegisterShipInitFunc initFunc(RegisterHealingMikauAudioFix, {});
