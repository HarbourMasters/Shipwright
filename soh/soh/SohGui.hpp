//
//  SohGui.hpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#ifndef SohGui_hpp
#define SohGui_hpp

#include <stdio.h>
#include "SohMenuBar.h"
#include "Enhancements/audio/AudioEditor.h"
#include "Enhancements/controls/GameControlEditor.h"
#include "Enhancements/cosmetics/CosmeticsEditor.h"
#include "Enhancements/debugger/actorViewer.h"
#include "Enhancements/debugger/colViewer.h"
#include "Enhancements/debugger/debugSaveEditor.h"
#include "Enhancements/debugger/dlViewer.h"
#include "Enhancements/gameplaystatswindow.h"
#include "Enhancements/randomizer/randomizer_check_tracker.h"
#include "Enhancements/randomizer/randomizer_entrance_tracker.h"

#ifdef __cplusplus
extern "C" {
#endif
    void enableBetaQuest();
    void disableBetaQuest();
#ifdef __cplusplus
}
#endif

namespace SohGui {
    void SetupHooks();
    void SetupGuiElements();
    void Draw();
    std::shared_ptr<SohMenuBar> mSohMenuBar;
    std::shared_ptr<AudioEditor> mAudioEditorWindow;
    std::shared_ptr<GameControlEditor::GameControlEditorWindow> mGameControlEditorWindow;
    std::shared_ptr<CosmeticsEditorWindow> mCosmeticsEditorWindow;
    std::shared_ptr<ActorViewerWindow> mActorViewerWindow;
    std::shared_ptr<ColViewerWindow> mColViewerWindow;
    std::shared_ptr<SaveEditorWindow> mSaveEditorWindow;
    std::shared_ptr<DLViewerWindow> mDLViewerWindow;
    std::shared_ptr<GameplayStatsWindow> mGameplayStatsWindow;
    std::shared_ptr<CheckTracker::CheckTrackerSettingsWindow> mCheckTrackerSettingsWindow;
    std::shared_ptr<CheckTracker::CheckTrackerWindow> mCheckTrackerWindow;
    std::shared_ptr<EntranceTrackerWindow> mEntranceTrackerWindow;
}

#endif /* SohGui_hpp */
