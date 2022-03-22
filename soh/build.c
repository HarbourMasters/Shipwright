const char gBuildVersion[] = "DECKARD";
const char gBuildTeam[] = "github.com/harbourmasters";
#ifdef __TIMESTAMP__
const char gBuildDate[] = __TIMESTAMP__;
#else
const char gBuildDate[] = __DATE__ " " __TIME__;
#endif
const char gBuildMakeOption[] = "";