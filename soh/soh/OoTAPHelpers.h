#include <libultraship/libultraship.h>

extern int64_t OoTAP_APItemToGameItem(int64_t item);
extern std::string OoTAP_GetItemName(int64_t item);
extern void OoTAP_InitCheckLookup();
extern std::vector<int64_t> const& OoTAP_GetSupportedLocations();
extern int64_t OoTAP_RandoCheckToAPCheck(RandomizerCheck check);
extern RandomizerCheck OoTAP_APCheckToRandoCheck(int64_t id);
