#ifndef SAVING_ENHANCEMENTS_H
#define SAVING_ENHANCEMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

void SavingEnhancements_PersistSaveEntranceInfo();
void SavingEnhancements_ClearSaveEntranceInfo();
bool SavingEnhancements_CanSave();
void SavingEnhancements_AdvancePlaytime();

#ifdef __cplusplus
}
#endif

#endif // SAVING_ENHANCEMENTS_H
