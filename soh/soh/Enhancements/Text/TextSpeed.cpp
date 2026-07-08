#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/ShipInit.hpp"

extern "C" {
#include "z64.h"
extern PlayState* gPlayState;
}

// Text Speed which fills whole box in one frame
static constexpr int32_t TEXT_SPEED_INSTANT = 6;

#define CVAR_TEXT_SPEED_NAME CVAR_ENHANCEMENT("TextSpeed")
#define CVAR_SLOW_TEXT_SPEED_NAME CVAR_ENHANCEMENT("SlowTextSpeed")

#define TEXT_SPEED CVarGetInteger(CVAR_TEXT_SPEED_NAME, 1)
#define SLOW_TEXT_SPEED CVarGetInteger(CVAR_SLOW_TEXT_SPEED_NAME, TEXT_SPEED)

static bool ShouldAdvanceQuickText(u16 textPos) {
    MessageContext* msgCtx = &gPlayState->msgCtx;

    if (textPos + TEXT_SPEED < msgCtx->textDrawPos) {
        return false;
    }

    if (msgCtx->msgMode == MSGMODE_TEXT_DISPLAYING ||
        (msgCtx->msgMode >= MSGMODE_OCARINA_STARTING && msgCtx->msgMode < MSGMODE_SCARECROW_LONG_RECORDING_START)) {
        return true;
    }

    return false;
}

static void FastTextCrawl(u16 textPos, bool* should) {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    if (msgCtx->textDelay == 0) {
        msgCtx->textDrawPos = textPos + TEXT_SPEED;
        if (msgCtx->textDrawPos > msgCtx->decodedTextLen) {
            msgCtx->textDrawPos = msgCtx->decodedTextLen + 1;
        }
        *should = true;
    }
}

static void SlowTextCrawl(bool* should) {
    MessageContext* msgCtx = &gPlayState->msgCtx;
    if (msgCtx->textDelayTimer <= 0) {
        return;
    }
    *should = true;
    if (msgCtx->textDelayTimer > SLOW_TEXT_SPEED) {
        msgCtx->textDelayTimer -= SLOW_TEXT_SPEED;
    } else {
        msgCtx->textDelayTimer = 0;
    }
}

static void RegisterTextSpeedModifiers() {
    COND_VB_SHOULD(VB_ENABLE_QUICKTEXT, TEXT_SPEED > 1, {
        u16 textPos = va_arg(args, int);
        if (!*should && ShouldAdvanceQuickText(textPos)) {
            *should = true;
        }
    });

    COND_VB_SHOULD(VB_FIX_TEXT_SPEED_SOFTLOCK, TEXT_SPEED > 1, {
        MessageContext* msgCtx = &gPlayState->msgCtx;
        u16 nextTextPos = va_arg(args, int);

        *should = !*should || (nextTextPos > msgCtx->textDrawPos);
        if (*should) {
            msgCtx->textDrawPos = nextTextPos;
        }
    });

    COND_VB_SHOULD(VB_TEXT_CRAWL_FASTER, TEXT_SPEED >= TEXT_SPEED_INSTANT, {
        MessageContext* msgCtx = &gPlayState->msgCtx;
        msgCtx->textDrawPos = msgCtx->decodedTextLen + 1;
        *should = true;
    });

    COND_VB_SHOULD(VB_TEXT_CRAWL_FASTER, TEXT_SPEED > 1 && TEXT_SPEED < TEXT_SPEED_INSTANT, {
        u16 textPos = va_arg(args, int);
        FastTextCrawl(textPos, should);
    });

    COND_VB_SHOULD(VB_TEXT_CRAWL_FASTER, SLOW_TEXT_SPEED > 1 && TEXT_SPEED < TEXT_SPEED_INSTANT,
                   { SlowTextCrawl(should); });
}

static RegisterShipInitFunc initFunc(RegisterTextSpeedModifiers, { CVAR_TEXT_SPEED_NAME, CVAR_SLOW_TEXT_SPEED_NAME });
