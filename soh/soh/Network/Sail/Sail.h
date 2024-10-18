#ifdef ENABLE_REMOTE_CONTROL
#ifndef NETWORK_SAIL_H
#define NETWORK_SAIL_H
#ifdef __cplusplus

#include "soh/Network/Network.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

class Sail : public Network {
  private:
    GameInteractionEffectBase* EffectFromJson(nlohmann::json payload);
    void RegisterHooks();

  public:
    static Sail* Instance;

    void Enable();
    void OnIncomingJson(nlohmann::json payload);
    void OnConnected();
    void OnDisconnected();
    void DrawMenu();
};

#endif // __cplusplus
#endif // NETWORK_SAIL_H
#endif // ENABLE_REMOTE_CONTROL
