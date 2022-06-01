#include "global.h"

#include <string.h>

#include "message_data_static.h"
#include "textures/nes_font_static/nes_font_static.h"
#include "textures/message_static/message_static.h"

static const char* fntTbl[] =
{
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
    gMsgChar7FBlankTex,
    gMsgChar80LatinCapitalLetterAWithGraveTex,
    gMsgChar81LatinCapitalLetterIWithCircumflexTex,
    gMsgChar82LatinCapitalLetterAWithCircumflexTex,
    gMsgChar83LatinCapitalLetterAWithDiaeresisTex,
    gMsgChar84LatinCapitalLetterCWithCedillaTex,
    gMsgChar85LatinCapitalLetterEWithGraveTex,
    gMsgChar86LatinCapitalLetterEWithAcuteTex,
    gMsgChar87LatinCapitalLetterEWithCircumflexTex,
    gMsgChar88LatinCapitalLetterEWithDiaeresisTex,
    gMsgChar89LatinCapitalLetterIWithDiaeresisTex,
    gMsgChar8ALatinCapitalLetterOWithCircumflexTex,
    gMsgChar8BLatinCapitalLetterOWithDiaeresisTex,
    gMsgChar8CLatinCapitalLetterUWithGraveTex,
    gMsgChar8DLatinCapitalLetterUWithCircumflexTex,
    gMsgChar8ELatinCapitalLetterUWithDiaeresisTex,
    gMsgChar8FLatinSmallLetterSharpSTex,
    gMsgChar90LatinSmallLetterAWithGraveTex,
    gMsgChar91LatinSmallLetterAWithAcuteTex,
    gMsgChar92LatinSmallLetterAWithCircumflexTex,
    gMsgChar93LatinSmallLetterAWithDiaeresisTex,
    gMsgChar94LatinSmallLetterCWithCedillaTex,
    gMsgChar95LatinSmallLetterEWithGraveTex,
    gMsgChar96LatinSmallLetterEWithAcuteTex,
    gMsgChar97LatinSmallLetterEWithCircumflexTex,
    gMsgChar98LatinSmallLetterEWithDiaeresisTex,
    gMsgChar99LatinSmallLetterIWithDiaeresisTex,
    gMsgChar9ALatinSmallLetterOWithCircumflexTex,
    gMsgChar9BLatinSmallLetterOWithDiaeresisTex,
    gMsgChar9CLatinSmallLetterUWithGraveTex,
    gMsgChar9DLatinSmallLetterUWithCircumflexTex,
    gMsgChar9ELatinSmallLetterUWithDiaeresisTex,
    gMsgChar9FButtonATex,
    gMsgCharA0ButtonBTex,
    gMsgCharA1ButtonCTex,
    gMsgCharA2ButtonLTex,
    gMsgCharA3ButtonRTex,
    gMsgCharA4ButtonZTex,
    gMsgCharA5ButtonCUpTex,
    gMsgCharA6ButtonCDownTex,
    gMsgCharA7ButtonCLeftTex,
    gMsgCharA8ButtonCRightTex,
    gMsgCharA9ZTargetSignTex,
    gMsgCharAAControlStickTex,
    gMsgCharABControlPadTex,
};

const char* msgStaticTbl[] =
{
    gDefaultMessageBackgroundTex,
    gSignMessageBackgroundTex,
    gNoteStaffMessageBackgroundTex,
    gFadingMessageBackgroundTex,
    gMessageContinueTriangleTex,
    gMessageEndSquareTex,
    gMessageArrowTex
};

void func_8006EE50(Font* font, u16 arg1, u16 arg2) {
}

/**
 * Loads a texture from nes_font_static for the requested `character` into the character texture buffer
 * at `codePointIndex`. The value of `character` is the ASCII codepoint subtract ' '/0x20.
 */
void Font_LoadChar(Font* font, u8 character, u16 codePointIndex) {
    //DmaMgr_SendRequest1(&font->charTexBuf[codePointIndex],
                        //&_nes_font_staticSegmentRomStart[character * FONT_CHAR_TEX_SIZE], FONT_CHAR_TEX_SIZE,
                        //"../z_kanfont.c", 93);

    if (character < 0x8B)
        memcpy(&font->charTexBuf[codePointIndex], ResourceMgr_LoadTexByName(fntTbl[character]), FONT_CHAR_TEX_SIZE);
}

/**
 * Loads a message box icon from message_static, such as the ending triangle/square or choice arrow into the
 * icon buffer.
 * The different icons are given in the MessageBoxIcon enum.
 */
void Font_LoadMessageBoxIcon(Font* font, u16 icon) {
    memcpy(font->iconBuf, ResourceMgr_LoadTexByName(msgStaticTbl[4 + icon]), FONT_CHAR_TEX_SIZE);
}

/**
 * Loads a full set of character textures based on their ordering in the message with text id 0xFFFC into
 * the font buffer.
 */
void Font_LoadOrderedFont(Font* font) {
    size_t len;
    size_t jj;
    s32 fontStatic;
    u8* fontBuf;
    s32 codePointIndex;
    s32 fontBufIndex;
    s32 offset;

    len = strlen(_message_0xFFFC_nes);
    memcpy(font->msgBuf, _message_0xFFFC_nes, len);

    osSyncPrintf("msg_data=%x,  msg_data0=%x   jj=%x\n", font->msgOffset, font->msgLength, jj = len);

    len = jj;
    for (fontBufIndex = 0, codePointIndex = 0; font->msgBuf[codePointIndex] != MESSAGE_END; codePointIndex++) {
        if (codePointIndex > len) {
            osSyncPrintf("ＥＲＲＯＲ！！  エラー！！！  error───！！！！\n");
            return;
        }

        if (font->msgBuf[codePointIndex] != MESSAGE_NEWLINE) {
            fontBuf = font->fontBuf + fontBufIndex * 8;
            fontStatic = _nes_font_staticSegmentRomStart;

            osSyncPrintf("nes_mes_buf[%d]=%d\n", codePointIndex, font->msgBuf[codePointIndex]);

            offset = (font->msgBuf[codePointIndex] - '\x20') * FONT_CHAR_TEX_SIZE;
            memcpy(fontBuf, ResourceMgr_LoadTexByName(fntTbl[offset / FONT_CHAR_TEX_SIZE]), FONT_CHAR_TEX_SIZE);
            //DmaMgr_SendRequest1(fontBuf, fontStatic + offset, FONT_CHAR_TEX_SIZE, "../z_kanfont.c", 134);
            fontBufIndex += FONT_CHAR_TEX_SIZE / 8;
        }
    }
}
