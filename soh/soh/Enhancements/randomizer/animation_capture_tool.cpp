// I provide here the code which allowed me to have all the data for the roll animation for roll_animation_data.h
// I had temporarily put it in the Draw.cpp file and I called the function when Link performed a roll

/*
#define NUM_FRAMES_TO_CAPTURE 10
static Vec3s capturedRollFrames[NUM_FRAMES_TO_CAPTURE][LIMB_COUNT_LINK];
static int captureFrameCount = 0;
static bool captureComplete = false;
static bool captureStarted = false;

extern "C" void CaptureRollAnimation(PlayState* play) {
    if (captureComplete)
        return;

    Player* player = GET_PLAYER(play);
    if (player == NULL || player->skelAnime.jointTable == NULL)
        return;

    // Checks if the player is rolling | Vérifie si le joueur roule
    bool isRolling = (player->actionFunc == Player_Action_Roll);

    if (isRolling) {
        if (!captureStarted) {
            captureStarted = true;
            osSyncPrintf("\n========== DÉBUT CAPTURE ANIMATION ROLL ==========\n");
        }

        // Captures a frame every 2 game frames | Capture une frame toutes les 2 frames de jeu
        if ((play->state.frames % 2) == 0 && captureFrameCount < NUM_FRAMES_TO_CAPTURE) {
            osSyncPrintf("\n// Frame %d\n", captureFrameCount);
            osSyncPrintf("{\n");

            for (int i = 0; i < LIMB_COUNT_LINK; i++) {
                Vec3s joint = player->skelAnime.jointTable[i];
                capturedRollFrames[captureFrameCount][i] = joint;

                osSyncPrintf("    { %d, %d, %d },  // Limb %d\n", joint.x, joint.y, joint.z, i);
            }

            osSyncPrintf("},\n");

            captureFrameCount++;

            if (captureFrameCount >= NUM_FRAMES_TO_CAPTURE) {
                captureComplete = true;
                osSyncPrintf("\n========== FIN CAPTURE - %d FRAMES CAPTURÉES ==========\n", NUM_FRAMES_TO_CAPTURE);
                osSyncPrintf("Copie ces données et envoie-les moi !\n\n");
            }
        }
    }
}
*/