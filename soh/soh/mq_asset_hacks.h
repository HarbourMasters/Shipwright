#ifndef MQ_ASSET_HACKS_H
#define MQ_ASSET_HACKS_H

#include "align_asset_macro.h"

#define dgYdanTex_00BA18_MQ "__OTR__scenes/mq/ydan_scene/gYdanTex_00BA18"
static const ALIGN_ASSET(2) char gYdanTex_00BA18_MQ[] = dgYdanTex_00BA18_MQ;

#define dgYdanTex_00CA18_MQ "__OTR__scenes/mq/ydan_scene/gYdanTex_00CA18"
static const ALIGN_ASSET(2) char gYdanTex_00CA18_MQ[] = dgYdanTex_00CA18_MQ;
void* D_8012A2F8_MQ[] = {
    gYdanTex_00BA18_MQ,
    gYdanTex_00CA18_MQ,
};

#define dgGoronCityNightEntranceTex_MQ "__OTR__scenes/mq/spot18_scene/gGoronCityNightEntranceTex"
static const ALIGN_ASSET(2) char gGoronCityNightEntranceTex_MQ[] = dgGoronCityNightEntranceTex_MQ;

#define dgGoronCityDayEntranceTex_MQ "__OTR__scenes/mq/spot18_scene/gGoronCityDayEntranceTex"
static const ALIGN_ASSET(2) char gGoronCityDayEntranceTex_MQ[] = dgGoronCityDayEntranceTex_MQ;
void* sGoronCityEntranceTextures_MQ[] = {
    gGoronCityDayEntranceTex_MQ,
    gGoronCityNightEntranceTex_MQ,
};

#define dgLonLonRanchDayWindowTex_MQ "__OTR__scenes/mq/spot20_scene/gLonLonRanchDayWindowTex"
static const ALIGN_ASSET(2) char gLonLonRanchDayWindowTex_MQ[] = dgLonLonRanchDayWindowTex_MQ;

#define dgLonLonRangeNightWindowsTex_MQ "__OTR__scenes/mq/spot20_scene/gLonLonRangeNightWindowsTex"
static const ALIGN_ASSET(2) char gLonLonRangeNightWindowsTex_MQ[] = dgLonLonRangeNightWindowsTex_MQ;
void* sLonLonRanchWindowTextures_MQ[] = {
    gLonLonRanchDayWindowTex_MQ,
    gLonLonRangeNightWindowsTex_MQ,
};

// walls of GF room where they throw you when you get caught
#define dgSpot12_009678Tex_MQ "__OTR__scenes/mq/spot12_scene/gSpot12_009678Tex"
static const ALIGN_ASSET(2) char gSpot12_009678Tex_MQ[] = dgSpot12_009678Tex_MQ;

#define dgSpot12_00DE78Tex_MQ "__OTR__scenes/mq/spot12_scene/gSpot12_00DE78Tex"
static const ALIGN_ASSET(2) char gSpot12_00DE78Tex_MQ[] = dgSpot12_00DE78Tex_MQ;
void* D_8012A380_MQ[] = {
    gSpot12_009678Tex_MQ,
    gSpot12_00DE78Tex_MQ,
};

#define dgZorasDomainDayEntranceTex_MQ "__OTR__scenes/mq/spot07_scene/gZorasDomainDayEntranceTex"
static const ALIGN_ASSET(2) char gZorasDomainDayEntranceTex_MQ[] = dgZorasDomainDayEntranceTex_MQ;

#define dgZorasDomainNightEntranceTex_MQ "__OTR__scenes/mq/spot07_scene/gZorasDomainNightEntranceTex"
static const ALIGN_ASSET(2) char gZorasDomainNightEntranceTex_MQ[] = dgZorasDomainNightEntranceTex_MQ;
void* sZorasDomainEntranceTextures_MQ[] = {
    gZorasDomainDayEntranceTex_MQ,
    gZorasDomainNightEntranceTex_MQ,
};

#define dgKakarikoVillageDayWindowTex_MQ "__OTR__scenes/mq/spot01_scene/gKakarikoVillageDayWindowTex"
static const ALIGN_ASSET(2) char gKakarikoVillageDayWindowTex_MQ[] = dgKakarikoVillageDayWindowTex_MQ;

#define dgKakarikoVillageNightWindowTex_MQ "__OTR__scenes/mq/spot01_scene/gKakarikoVillageNightWindowTex"
static const ALIGN_ASSET(2) char gKakarikoVillageNightWindowTex_MQ[] = dgKakarikoVillageNightWindowTex_MQ;
void* sKakarikoWindowTextures_MQ[] = {
    gKakarikoVillageDayWindowTex_MQ,
    gKakarikoVillageNightWindowTex_MQ,
};

#define dgGuardHouseOutSideView2NightTex_MQ "__OTR__scenes/mq/miharigoya_scene/gGuardHouseOutSideView2NightTex"
static const ALIGN_ASSET(2) char gGuardHouseOutSideView2NightTex_MQ[] = dgGuardHouseOutSideView2NightTex_MQ;

#define dgGuardHouseOutSideView2DayTex_MQ "__OTR__scenes/mq/miharigoya_scene/gGuardHouseOutSideView2DayTex"
static const ALIGN_ASSET(2) char gGuardHouseOutSideView2DayTex_MQ[] = dgGuardHouseOutSideView2DayTex_MQ;

#define dgGuardHouseOutSideView1NightTex_MQ "__OTR__scenes/mq/miharigoya_scene/gGuardHouseOutSideView1NightTex"
static const ALIGN_ASSET(2) char gGuardHouseOutSideView1NightTex_MQ[] = dgGuardHouseOutSideView1NightTex_MQ;

#define dgGuardHouseOutSideView1DayTex_MQ "__OTR__scenes/mq/miharigoya_scene/gGuardHouseOutSideView1DayTex"
static const ALIGN_ASSET(2) char gGuardHouseOutSideView1DayTex_MQ[] = dgGuardHouseOutSideView1DayTex_MQ;
void* sGuardHouseView2Textures_MQ[] = {
    gGuardHouseOutSideView1DayTex_MQ,
    gGuardHouseOutSideView1NightTex_MQ,
};
void* sGuardHouseView1Textures_MQ[] = {
    gGuardHouseOutSideView2DayTex_MQ,
    gGuardHouseOutSideView2NightTex_MQ,
};

#define dgLonLonHouseDayEntranceTex_MQ "__OTR__scenes/mq/souko_scene/gLonLonHouseDayEntranceTex"
static const ALIGN_ASSET(2) char gLonLonHouseDayEntranceTex_MQ[] = dgLonLonHouseDayEntranceTex_MQ;

#define dgLonLonHouseNightEntranceTex_MQ "__OTR__scenes/mq/souko_scene/gLonLonHouseNightEntranceTex"
static const ALIGN_ASSET(2) char gLonLonHouseNightEntranceTex_MQ[] = dgLonLonHouseNightEntranceTex_MQ;
void* sLonLonHouseEntranceTextures_MQ[] = {
    gLonLonHouseDayEntranceTex_MQ,
    gLonLonHouseNightEntranceTex_MQ,
};

#define dgThievesHideoutNightEntranceTex_MQ "__OTR__scenes/mq/gerudoway_scene/gThievesHideoutNightEntranceTex"
static const ALIGN_ASSET(2) char gThievesHideoutNightEntranceTex_MQ[] = dgThievesHideoutNightEntranceTex_MQ;

#define dgThievesHideoutDayEntranceTex_MQ "__OTR__scenes/mq/gerudoway_scene/gThievesHideoutDayEntranceTex"
static const ALIGN_ASSET(2) char gThievesHideoutDayEntranceTex_MQ[] = dgThievesHideoutDayEntranceTex_MQ;
void* sThievesHideoutEntranceTextures_MQ[] = {
    gThievesHideoutDayEntranceTex_MQ,
    gThievesHideoutNightEntranceTex_MQ,
};

#define dgDCDayEntranceTex_MQ "__OTR__scenes/mq/ddan_scene/gDCDayEntranceTex"
static const ALIGN_ASSET(2) char gDCDayEntranceTex_MQ[] = dgDCDayEntranceTex_MQ;

#define dgDCNightEntranceTex_MQ "__OTR__scenes/mq/ddan_scene/gDCNightEntranceTex"
static const ALIGN_ASSET(2) char gDCNightEntranceTex_MQ[] = dgDCNightEntranceTex_MQ;
void* gDCEntranceTextures_MQ[] = {
    gDCDayEntranceTex_MQ,
    gDCNightEntranceTex_MQ,
};

#define dgDCLavaFloor1Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor1Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor1Tex_MQ[] = dgDCLavaFloor1Tex_MQ;

#define dgDCLavaFloor2Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor2Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor2Tex_MQ[] = dgDCLavaFloor2Tex_MQ;

#define dgDCLavaFloor3Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor3Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor3Tex_MQ[] = dgDCLavaFloor3Tex_MQ;

#define dgDCLavaFloor4Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor4Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor4Tex_MQ[] = dgDCLavaFloor4Tex_MQ;

#define dgDCLavaFloor5Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor5Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor5Tex_MQ[] = dgDCLavaFloor5Tex_MQ;

#define dgDCLavaFloor6Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor6Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor6Tex_MQ[] = dgDCLavaFloor6Tex_MQ;

#define dgDCLavaFloor7Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor7Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor7Tex_MQ[] = dgDCLavaFloor7Tex_MQ;

#define dgDCLavaFloor8Tex_MQ "__OTR__scenes/mq/ddan_scene/gDCLavaFloor8Tex"
static const ALIGN_ASSET(2) char gDCLavaFloor8Tex_MQ[] = dgDCLavaFloor8Tex_MQ;
void* sDCLavaFloorTextures_MQ[] = {
    gDCLavaFloor1Tex_MQ, gDCLavaFloor2Tex_MQ, gDCLavaFloor3Tex_MQ, gDCLavaFloor4Tex_MQ,
    gDCLavaFloor5Tex_MQ, gDCLavaFloor6Tex_MQ, gDCLavaFloor7Tex_MQ, gDCLavaFloor8Tex_MQ,
};

#define dgWaterTempleDayEntranceTex_MQ "__OTR__scenes/mq/MIZUsin_scene/gWaterTempleDayEntranceTex"
static const ALIGN_ASSET(2) char gWaterTempleDayEntranceTex_MQ[] = dgWaterTempleDayEntranceTex_MQ;

#define dgWaterTempleNightEntranceTex_MQ "__OTR__scenes/mq/MIZUsin_scene/gWaterTempleNightEntranceTex"
static const ALIGN_ASSET(2) char gWaterTempleNightEntranceTex_MQ[] = dgWaterTempleNightEntranceTex_MQ;
void* D_8012A330_MQ[] = {
    gWaterTempleDayEntranceTex_MQ,
    gWaterTempleNightEntranceTex_MQ,
};

#define dgIceCavernNightEntranceTex_MQ "__OTR__scenes/mq/ice_doukutu_scene/gIceCavernNightEntranceTex"
static const ALIGN_ASSET(2) char gIceCavernNightEntranceTex_MQ[] = dgIceCavernNightEntranceTex_MQ;

#define dgIceCavernDayEntranceTex_MQ "__OTR__scenes/mq/ice_doukutu_scene/gIceCavernDayEntranceTex"
static const ALIGN_ASSET(2) char gIceCavernDayEntranceTex_MQ[] = dgIceCavernDayEntranceTex_MQ;
void* sIceCavernEntranceTextures_MQ[] = {
    gIceCavernDayEntranceTex_MQ,
    gIceCavernNightEntranceTex_MQ,
};

#define dgGTGDayEntranceTex_MQ "__OTR__scenes/mq/men_scene/gGTGDayEntranceTex"
static const ALIGN_ASSET(2) char gGTGDayEntranceTex_MQ[] = dgGTGDayEntranceTex_MQ;

#define dgGTGNightEntranceTex_MQ "__OTR__scenes/mq/men_scene/gGTGNightEntranceTex"
static const ALIGN_ASSET(2) char gGTGNightEntranceTex_MQ[] = dgGTGNightEntranceTex_MQ;
void* sGTGEntranceTextures_MQ[] = {
    gGTGDayEntranceTex_MQ,
    gGTGNightEntranceTex_MQ,
};

#define dgForestTempleDayEntranceTex_MQ "__OTR__scenes/mq/Bmori1_scene/gForestTempleDayEntranceTex"
static const ALIGN_ASSET(2) char gForestTempleDayEntranceTex_MQ[] = dgForestTempleDayEntranceTex_MQ;

#define dgForestTempleNightEntranceTex_MQ "__OTR__scenes/mq/Bmori1_scene/gForestTempleNightEntranceTex"
static const ALIGN_ASSET(2) char gForestTempleNightEntranceTex_MQ[] = dgForestTempleNightEntranceTex_MQ;
void* sForestTempleEntranceTextures_MQ[] = {
    gForestTempleDayEntranceTex_MQ,
    gForestTempleNightEntranceTex_MQ,
};

#define dgSpiritTempleDayEntranceTex_MQ "__OTR__scenes/mq/jyasinzou_scene/gSpiritTempleDayEntranceTex"
static const ALIGN_ASSET(2) char gSpiritTempleDayEntranceTex_MQ[] = dgSpiritTempleDayEntranceTex_MQ;

#define dgSpiritTempleNightEntranceTex_MQ "__OTR__scenes/mq/jyasinzou_scene/gSpiritTempleNightEntranceTex"
static const ALIGN_ASSET(2) char gSpiritTempleNightEntranceTex_MQ[] = dgSpiritTempleNightEntranceTex_MQ;
void* sSpiritTempleEntranceTextures_MQ[] = {
    gSpiritTempleDayEntranceTex_MQ,
    gSpiritTempleNightEntranceTex_MQ,
};

#endif
