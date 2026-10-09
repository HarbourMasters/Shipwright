#include "global.h"
#include "BenPort.h"
#include "assets/interface/nes_font_static/nes_font_static.h"
#include "assets/interface/message_static/message_static.h"
#include <string.h>

// #region 2S2H [Port] Asset tables we can pull from instead of from ROM
const char* fontTbl[156] = {
    gMsgChar20SpaceTex,
    gMsgChar21ExclamationMarkTex,
    gMsgChar22QuotationMarkTex,
    gMsgChar23NumberSignTex,
    gMsgChar24DollarSignTex,
    gMsgChar25PercentSignTex,
    gMsgChar26AmpersandTex,
    gMsgChar27ApostropheTex,
    gMsgChar28LeftParenthesesTex,
    gMsgChar29RightParenthesesTex,
    gMsgChar2AAsteriskTex,
    gMsgChar2BPlusSignTex,
    gMsgChar2CCommaTex,
    gMsgChar2DHyphenMinusTex,
    gMsgChar2EFullStopTex,
    gMsgChar2FSolidusTex,
    gMsgChar30Digit0Tex,
    gMsgChar31Digit1Tex,
    gMsgChar32Digit2Tex,
    gMsgChar33Digit3Tex,
    gMsgChar34Digit4Tex,
    gMsgChar35Digit5Tex,
    gMsgChar36Digit6Tex,
    gMsgChar37Digit7Tex,
    gMsgChar38Digit8Tex,
    gMsgChar39Digit9Tex,
    gMsgChar3AColonTex,
    gMsgChar3BSemicolonTex,
    gMsgChar3CLessThanSignTex,
    gMsgChar3DEqualsSignTex,
    gMsgChar3EGreaterThanSignTex,
    gMsgChar3FQuestionMarkTex,
    gMsgChar40CommercialAtTex,
    gMsgChar41LatinCapitalLetterATex,
    gMsgChar42LatinCapitalLetterBTex,
    gMsgChar43LatinCapitalLetterCTex,
    gMsgChar44LatinCapitalLetterDTex,
    gMsgChar45LatinCapitalLetterETex,
    gMsgChar46LatinCapitalLetterFTex,
    gMsgChar47LatinCapitalLetterGTex,
    gMsgChar48LatinCapitalLetterHTex,
    gMsgChar49LatinCapitalLetterITex,
    gMsgChar4ALatinCapitalLetterJTex,
    gMsgChar4BLatinCapitalLetterKTex,
    gMsgChar4CLatinCapitalLetterLTex,
    gMsgChar4DLatinCapitalLetterMTex,
    gMsgChar4ELatinCapitalLetterNTex,
    gMsgChar4FLatinCapitalLetterOTex,
    gMsgChar50LatinCapitalLetterPTex,
    gMsgChar51LatinCapitalLetterQTex,
    gMsgChar52LatinCapitalLetterRTex,
    gMsgChar53LatinCapitalLetterSTex,
    gMsgChar54LatinCapitalLetterTTex,
    gMsgChar55LatinCapitalLetterUTex,
    gMsgChar56LatinCapitalLetterVTex,
    gMsgChar57LatinCapitalLetterWTex,
    gMsgChar58LatinCapitalLetterXTex,
    gMsgChar59LatinCapitalLetterYTex,
    gMsgChar5ALatinCapitalLetterZTex,
    gMsgChar5BLeftSquareBracketTex,
    gMsgChar5CYenSignTex,
    gMsgChar5DRightSquareBracketTex,
    gMsgChar5ECircumflexAccentTex,
    gMsgChar5FLowLineTex,
    gMsgChar60GraveAccentTex,
    gMsgChar61LatinSmallLetterATex,
    gMsgChar62LatinSmallLetterBTex,
    gMsgChar63LatinSmallLetterCTex,
    gMsgChar64LatinSmallLetterDTex,
    gMsgChar65LatinSmallLetterETex,
    gMsgChar66LatinSmallLetterFTex,
    gMsgChar67LatinSmallLetterGTex,
    gMsgChar68LatinSmallLetterHTex,
    gMsgChar69LatinSmallLetterITex,
    gMsgChar6ALatinSmallLetterJTex,
    gMsgChar6BLatinSmallLetterKTex,
    gMsgChar6CLatinSmallLetterLTex,
    gMsgChar6DLatinSmallLetterMTex,
    gMsgChar6ELatinSmallLetterNTex,
    gMsgChar6FLatinSmallLetterOTex,
    gMsgChar70LatinSmallLetterPTex,
    gMsgChar71LatinSmallLetterQTex,
    gMsgChar72LatinSmallLetterRTex,
    gMsgChar73LatinSmallLetterSTex,
    gMsgChar74LatinSmallLetterTTex,
    gMsgChar75LatinSmallLetterUTex,
    gMsgChar76LatinSmallLetterVTex,
    gMsgChar77LatinSmallLetterWTex,
    gMsgChar78LatinSmallLetterXTex,
    gMsgChar79LatinSmallLetterYTex,
    gMsgChar7ALatinSmallLetterZTex,
    gMsgChar7BLeftCurlyBracketTex,
    gMsgChar7CVerticalLineTex,
    gMsgChar7DRightCurlyBracketTex,
    gMsgChar7ETildeTex,
    gMsgChar7FMasculineOrdinalIndicatorTex,
    gMsgChar80LatinCapitalLetterAWithGraveTex,
    gMsgChar81LatinCapitalLetterAWithAcuteTex,
    gMsgChar82LatinCapitalLetterAWithCircumflexTex,
    gMsgChar83LatinCapitalLetterAWithDiaeresisTex,
    gMsgChar84LatinCapitalLetterCWithCedillaTex,
    gMsgChar85LatinCapitalLetterEWithGraveTex,
    gMsgChar86LatinCapitalLetterEWithAcuteTex,
    gMsgChar87LatinCapitalLetterEWithCircumflexTex,
    gMsgChar88LatinCapitalLetterEWithDiaeresisTex,
    gMsgChar89LatinCapitalLetterIWithGraveTex,
    gMsgChar8ALatinCapitalLetterIWithAcuteTex,
    gMsgChar8BLatinCapitalLetterIWithCircumflexTex,
    gMsgChar8CLatinCapitalLetterIWithDiaeresisTex,
    gMsgChar8DLatinCapitalLetterNWithTildeTex,
    gMsgChar8ELatinCapitalLetterOWithGraveTex,
    gMsgChar8FLatinCapitalLetterOWithAcuteTex,
    gMsgChar90LatinCapitalLetterOWithCircumflexTex,
    gMsgChar91LatinCapitalLetterOWithDiaeresisTex,
    gMsgChar92LatinCapitalLetterUWithGraveTex,
    gMsgChar93LatinCapitalLetterUWithAcuteTex,
    gMsgChar94LatinCapitalLetterUWithCircumflexTex,
    gMsgChar95LatinCapitalLetterUWithDiaeresisTex,
    gMsgChar96GreekSmallLetterBetaTex,
    gMsgChar97LatinSmallLetterAWithGraveTex,
    gMsgChar98LatinSmallLetterAWithAcuteTex,
    gMsgChar99LatinSmallLetterAWithCircumflexTex,
    gMsgChar9ALatinSmallLetterAWithDiaeresisTex,
    gMsgChar9BLatinSmallLetterCWithCedillaTex,
    gMsgChar9CLatinSmallLetterEWithGraveTex,
    gMsgChar9DLatinSmallLetterEWithAcuteTex,
    gMsgChar9ELatinSmallLetterEWithCircumflexTex,
    gMsgChar9FLatinSmallLetterEWithDiaeresisTex,
    gMsgCharA0LatinSmallLetterIWithGraveTex,
    gMsgCharA1LatinSmallLetterIWithAcuteTex,
    gMsgCharA2LatinSmallLetterIWithCircumflexTex,
    gMsgCharA3LatinSmallLetterIWithDiaeresisTex,
    gMsgCharA4LatinSmallLetterNWithTildeTex,
    gMsgCharA5LatinSmallLetterOWithGraveTex,
    gMsgCharA6LatinSmallLetterOWithAcuteTex,
    gMsgCharA7LatinSmallLetterOWithCircumflexTex,
    gMsgCharA8LatinSmallLetterOWithDiaeresisTex,
    gMsgCharA9LatinSmallLetterUWithGraveTex,
    gMsgCharAALatinSmallLetterUWithAcuteTex,
    gMsgCharABLatinSmallLetterUWithCircumflexTex,
    gMsgCharACLatinSmallLetterUWithDiaeresisTex,
    gMsgCharADInvertedExclamationMarkTex,
    gMsgCharAEInvertedQuestionMarkTex,
    gMsgCharAFFeminineOrdinalIndicatorTex,
    gMsgCharB0ButtonATex,
    gMsgCharB1ButtonBTex,
    gMsgCharB2ButtonCTex,
    gMsgCharB3ButtonLTex,
    gMsgCharB4ButtonRTex,
    gMsgCharB5ButtonZTex,
    gMsgCharB6ButtonCUpTex,
    gMsgCharB7ButtonCDownTex,
    gMsgCharB8ButtonCLeftTex,
    gMsgCharB9ButtonCRightTex,
    gMsgCharBAZTargetSignTex,
    gMsgCharBBControlStickTex,
};

const char* gMessageBoxEndIcons[] = {
    gMessageContinueTriangleTex,
    gMessageEndSquareTex,
    gMessageArrowTex,
};
// #endregion

// stubbed in NTSC-U
void Font_LoadChar(PlayState* play, u16 codePointIndex, s32 offset) {
}

void Font_LoadCharNES(PlayState* play, u8 codePointIndex, s32 offset) {
    MessageContext* msgCtx = &play->msgCtx;
    Font* font = &msgCtx->font;

    int fontIdx = codePointIndex - 0x20;

    if (fontIdx >= 0 && fontIdx < ARRAY_COUNT(fontTbl)) {
        memcpy(&font->charBuf[font->unk_11D88][offset], fontTbl[fontIdx], strlen(fontTbl[fontIdx]) + 1);
    }

    // DmaMgr_SendRequest0(&font->charBuf[font->unk_11D88][offset],
    //&((u8*)SEGMENT_ROM_START(nes_font_static))[(codePointIndex - ' ') * FONT_CHAR_TEX_SIZE],
    // FONT_CHAR_TEX_SIZE);
}

void Font_LoadMessageBoxEndIcon(Font* font, u16 icon) {
    // #region 2S2H [Port]
    // DmaMgr_SendRequest0(&font->iconBuf,
    //                     SEGMENT_ROM_START_OFFSET(message_static, 5 * 0x1000 + icon * FONT_CHAR_TEX_SIZE),
    //                     FONT_CHAR_TEX_SIZE);
    memcpy(&font->iconBuf, gMessageBoxEndIcons[icon], strlen(gMessageBoxEndIcons[icon]) + 1);
    // #endregion
}

static u8 sFontOrdering[] = {
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x41, 0x42,
    0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55,
    0x56, 0x57, 0x58, 0x59, 0x5A, 0x00, 0x0D, 0x0E, 0x1A, 0x61, 0x66, 0x6A, 0x6D, 0x6F, 0x73, 0x76, 0x77, 0x78, 0x79,
    0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F, 0x80, 0x81, 0x84, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C,
};

void Font_LoadOrderedFont(Font* font) {
    u32 loadOffset;
    s32 codePointIndex = 0;
    u8* writeLocation;

    while (1) {
        writeLocation = &font->fontBuf[codePointIndex * FONT_CHAR_TEX_SIZE];
        // #region 2S2H [Port]
        loadOffset = sFontOrdering[codePointIndex]; // * FONT_CHAR_TEX_SIZE;
        if (sFontOrdering[codePointIndex] == 0) {
            loadOffset = 0;
        }

        // DmaMgr_SendRequest0(writeLocation, (uintptr_t)SEGMENT_ROM_START(nes_font_static) + loadOffset,
        // FONT_CHAR_TEX_SIZE);
        memcpy(writeLocation, fontTbl[loadOffset], strlen(fontTbl[loadOffset]) + 1);
        // #endregion

        if (sFontOrdering[codePointIndex] == 0x8C) {
            break;
        }
        codePointIndex++;
    }
}
