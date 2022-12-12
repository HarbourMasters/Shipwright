//
//  GameMenuBar.hpp
//  soh
//
//  Created by David Chavez on 24.08.22.
//

#ifndef GameMenuBar_hpp
#define GameMenuBar_hpp

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
    void enableBetaQuest();
    void disableBetaQuest();
#ifdef __cplusplus
}
#endif

#include "Enhancements/online/Online.h"

namespace GameMenuBar {
    void SetupHooks();
    void Draw();
}

#endif /* GameMenuBar_hpp */
