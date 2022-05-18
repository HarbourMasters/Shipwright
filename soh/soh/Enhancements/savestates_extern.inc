extern "C" MtxF* sMatrixStack;
extern "C" MtxF* sCurrentMatrix;
extern "C" LightsBuffer sLightsBuffer;
extern "C" s16 sWarpTimerTarget;
extern "C" MapMarkData** sLoadedMarkDataTable;

//Camera static data
extern "C" int32_t sInitRegs;
extern "C" int32_t gDbgCamEnabled;
extern "C" int32_t sDbgModeIdx;
extern "C" int16_t sNextUID;
extern "C" int32_t sCameraInterfaceFlags;
extern "C" int32_t sCameraInterfaceAlpha;
extern "C" int32_t sCameraShrinkWindowVal;
extern "C" int32_t D_8011D3AC;
extern "C" int32_t sDemo5PrevAction12Frame;
extern "C" int32_t sDemo5PrevSfxFrame;
extern "C" int32_t D_8011D3F0;
extern "C" OnePointCsFull D_8011D6AC[];
extern "C" OnePointCsFull D_8011D724[];
extern "C" OnePointCsFull D_8011D79C[];
extern "C" OnePointCsFull D_8011D83C[];
extern "C" OnePointCsFull D_8011D88C[];
extern "C" OnePointCsFull D_8011D8DC[];
extern "C" OnePointCsFull D_8011D954[];
extern "C" OnePointCsFull D_8011D9F4[]; 
extern "C" int16_t D_8011DB08;
extern "C" int16_t D_8011DB0C;
extern "C" int32_t sOOBTimer;
extern "C" f32 D_8015CE50;
extern "C" f32 D_8015CE54;
extern "C" CamColChk D_8015CE58;

//Gameover
extern "C" uint16_t gGameOverTimer;

//One Point Demo
extern "C" uint32_t sPrevFrameCs1100;
extern "C" CutsceneCameraPoint D_8012013C[14];
extern "C" CutsceneCameraPoint D_8012021C[14];
extern "C" CutsceneCameraPoint D_801204D4[14];
extern "C" CutsceneCameraPoint D_801205B4[14];
extern "C" OnePointCsFull D_801208EC[3];
extern "C" OnePointCsFull D_80120964[2];
extern "C" OnePointCsFull D_801209B4[4];
extern "C" OnePointCsFull D_80120ACC[5];
extern "C" OnePointCsFull D_80120B94[11];
extern "C" OnePointCsFull D_80120D4C[7];
extern "C" OnePointCsFull D_80120FA4[6];
extern "C" OnePointCsFull D_80121184[2];
extern "C" OnePointCsFull D_801211D4[2];
extern "C" OnePointCsFull D_8012133C[3];
extern "C" OnePointCsFull D_801213B4[5];
extern "C" OnePointCsFull D_8012151C[2];
extern "C" OnePointCsFull D_8012156C[2];
extern "C" OnePointCsFull D_801215BC[1];
extern "C" OnePointCsFull D_80121C24[7];
extern "C" OnePointCsFull D_80121D3C[3];
extern "C" OnePointCsFull D_80121F1C[4];
extern "C" OnePointCsFull D_80121FBC[4];
extern "C" OnePointCsFull D_801220D4[5];
extern "C" OnePointCsFull D_80122714[4];
extern "C" OnePointCsFull D_80122CB4[2];
extern "C" OnePointCsFull D_80122D04[2];
extern "C" OnePointCsFull D_80122E44[2][7];
extern "C" OnePointCsFull D_8012313C[3];
extern "C" OnePointCsFull D_801231B4[4];
extern "C" OnePointCsFull D_80123254[2];
extern "C" OnePointCsFull D_801232A4[1];
extern "C" OnePointCsFull D_80123894[3];
extern "C" OnePointCsFull D_8012390C[2];
extern "C" OnePointCsFull D_8012395C[3];
extern "C" OnePointCsFull D_801239D4[3];

// z_bg_ddan_kd
extern "C" Vec3f sBgDdanKdVelocity;
extern "C" Vec3f sBgDdanKdAccel;

// z_bg_dodoago
extern "C" s16 sBgDodoagoFirstExplosiveFlag;
extern "C" u8 sBgDodoagoDisableBombCatcher;
extern "C" s32 sBgDodoagoTimer;

// z_bg_haka_trap
extern "C" uint32_t D_80880F30;
extern "C" uint32_t D_80881014;

// z_bg_hidan_rock
extern "C" float D_8088BFC0;

// z_bg_menkuri_eye
extern "C" int32_t D_8089C1A0;

// z_bg_mori_hineri
extern "C" int16_t sBgMoriHineriNextCamIdx;

// z_bg_po_event
extern "C" uint8_t sBgPoEventBlocksAtRest;
extern "C" uint8_t sBgPoEventPuzzleState;
extern "C" float sBgPoEventblockPushDist;

// z_bg_relay_objects
extern "C" uint32_t D_808A9508;

// z_bg_spot18_basket
extern "C" int16_t D_808B85D0;

// z_boss_ganon
extern "C" uint32_t sBossGanonSeed1;
extern "C" uint32_t sBossGanonSeed2;
extern "C" uint32_t sBossGanonSeed3;
extern "C" void* sBossGanonGanondorf;
extern "C" void* sBossGanonZelda;
extern "C" void* sBossGanonCape;
extern "C" GanondorfEffect sBossGanonEffectBuf[200];

// z_boss_ganon2
extern "C" Vec3f D_8090EB20;
extern "C" int8_t D_80910638;
extern "C" void* sBossGanon2Zelda;
extern "C" void* D_8090EB30;
extern "C" int32_t sBossGanon2Seed1;
extern "C" int32_t sBossGanon2Seed2;
extern "C" int32_t sBossGanon2Seed3;
extern "C" Vec3f D_809105D8[4];
extern "C" Vec3f D_80910608[4];
extern "C" BossGanon2Effect sBossGanon2Particles[100];

// z_boss_tw
extern "C" uint8_t sTwInitalized;
extern "C" BossTwEffect sTwEffects[150];

// z_demo_6k
extern "C" Vec3f sDemo6kVelocity;

// z_demo_du
extern "C" int32_t D_8096CE94;

// z_demo_kekkai
extern "C" Vec3f demoKekkaiVel;

// z_en_bw
extern "C" int32_t sSlugGroup;

// z_en_clear_tag
extern "C" uint8_t sClearTagIsEffectsInitialized;
extern "C" EnClearTagEffect sClearTagEffects[CLEAR_TAG_EFFECT_MAX_COUNT];

// z_en_fr
extern "C" EnFrPointers sEnFrPointers;

// z_en_goma
extern "C" uint8_t sSpawnNum;

// z_en_in
extern "C" int32_t D_80A7B998;

// z_en_insect
extern "C" float D_80A7DEB0;
extern "C" int16_t D_80A7DEB4;
extern "C" int16_t D_80A7DEB8;

// z_en_ishi
extern "C" int16_t sRockRotSpeedX;
extern "C" int16_t sRockRotSpeedY;

// z_en_niw
extern "C" int16_t D_80AB85E0;
extern "C" uint8_t sLowerRiverSpawned;
extern "C" uint8_t sUpperRiverSpawned;

// z_en_po_field
extern "C" int32_t sEnPoFieldNumSpawned;
extern "C" Vec3s sEnPoFieldSpawnPositions[10];
extern "C" u8 sEnPoFieldSpawnSwitchFlags[10];

// z_en_takara_man
extern "C" uint8_t sTakaraIsInitialized;

// z_en_xc
extern "C" int32_t D_80B41D90;
extern "C" int32_t sEnXcFlameSpawned;
extern "C" int32_t D_80B41DA8;
extern "C" int32_t D_80B41DAC;

// z_en_zf
extern "C" int16_t D_80B4A1B0;
extern "C" int16_t D_80B4A1B4;

extern "C" int32_t D_80B5A468;
extern "C" int32_t D_80B5A494;
extern "C" int32_t D_80B5A4BC;

extern "C" uint8_t sKankyoIsSpawned;
extern "C" int16_t sTrailingFairies;

extern "C" uint16_t gTimeIncrement;

extern "C" s16 sPlayerInitialPosX;
extern "C" s16 sPlayerInitialPosZ;
extern "C" s16 sPlayerInitialDirection;

// code_800EC960
// Related to ocarina
extern "C" u8 sOcarinaInpEnabled;
extern "C" s8 D_80130F10;
extern "C" u8 sCurOcarinaBtnVal;
extern "C" u8 sPrevOcarinaNoteVal;
extern "C" u8 sCurOcarinaBtnIdx;
extern "C" u8 sLearnSongLastBtn;
extern "C" f32 D_80130F24;
extern "C" f32 D_80130F28;
extern "C" s8 D_80130F2C;
extern "C" s8 D_80130F30;
extern "C" s8 D_80130F34;
extern "C" u8 sPlaybackState;
extern "C" u32 D_80130F3C;
extern "C" u32 sNotePlaybackTimer;
extern "C" u16 sPlaybackNotePos;
extern "C" u16 sStaffPlaybackPos;

//IDK what this is but it looks important 
extern "C" u32 sCurOcarinaBtnPress;
extern "C" u32 D_8016BA10;
extern "C" u32 sPrevOcarinaBtnPress;
extern "C" s32 D_8016BA18;
extern "C" s32 D_8016BA1C;
extern "C" u8 sCurOcarinaSong[8];
extern "C" u8 sOcarinaSongAppendPos;
extern "C" u8 sOcarinaHasStartedSong;
extern "C" u8 sOcarinaSongNoteStartIdx;
extern "C" u8 sOcarinaSongCnt;
extern "C" u16 sOcarinaAvailSongs;
extern "C" u8 sStaffPlayingPos;
extern "C" u16 sLearnSongPos[0x10];
extern "C" u16 D_8016BA50[0x10];
extern "C" u16 D_8016BA70[0x10];
extern "C" u8 sLearnSongExpectedNote[0x10];
extern "C" OcarinaNote D_8016BAA0;
extern "C" u8 sAudioHasMalonBgm;
extern "C" f32 sAudioMalonBgmDist;
extern "C" u8 sDisplayedNoteValue;



// z_message_PAL
extern "C" s16 sOcarinaNoteBufPos;
extern "C" s16 sOcarinaNoteBufLen;
extern "C" u8 sOcarinaNoteBuf[12];

extern "C" u8 D_8014B2F4;
extern "C" u8 sTextboxSkipped;
extern "C" u16 sNextTextId;
extern "C" s16 sLastPlayedSong;
extern "C" s16 sHasSunsSong;
extern "C" s16 sMessageHasSetSfx;
extern "C" u16 sOcarinaSongBitFlags;