#pragma once

#ifdef __cplusplus

#include <chrono>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

#include "soh/Network/Network.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"

class CrowdControl : public Network {
  private:
    enum EffectResult {
        /// <summary>The effect executed successfully.</summary>
        Success = 0x00,
        /// <summary>The effect failed to trigger, but is still available for use. Viewer(s) will be refunded. You
        /// probably don't want this.</summary>
        Failure = 0x01,
        /// <summary>Same as <see cref="Failure"/> but the effect is no longer available for use for the remainder of
        /// the game. You probably don't want this.</summary>
        Unavailable = 0x02,
        /// <summary>The effect cannot be triggered right now, try again in a few seconds. This is the "normal" failure
        /// response.</summary>
        Retry = 0x03,
        /// <summary>INTERNAL USE ONLY. The effect has been queued for execution after the current one ends.</summary>
        Queue = 0x04,
        /// <summary>INTERNAL USE ONLY. The effect triggered successfully and is now active until it ends.</summary>
        Running = 0x05,
        /// <summary>The timed effect has been paused and is now waiting.</summary>
        Paused = 0x06,
        /// <summary>The timed effect has been resumed and is counting down again.</summary>
        Resumed = 0x07,
        /// <summary>The timed effect has finished.</summary>
        Finished = 0x08,
        /// <summary>Effect is being initiated. SoH exclusive to check against if an effect state has changed or
        /// not.</summary>
        Initiate = 0xFE,
        /// <summary>The processor isn't ready to start or has shut down.</summary>
        NotReady = 0xFF
    };

    enum ResponseType {
        EffectRequest = 0x00,
        Login = 0xF0,
        LoginSuccess = 0xF1,
        Version = 0xFC,
        Disconnect = 0xFE,
        KeepAlive = 0xFF
    };

    struct Response {
        int id;
        EffectResult status;
        long timeRemaining;
        ResponseType type = ResponseType::EffectRequest;
    };

    typedef struct Effect {
        uint32_t id;
        uint32_t spawnParams[2];
        uint32_t category = 0;
        long timeRemaining;
        std::unique_ptr<GameInteractionEffectBase> giEffect;
        std::string viewerName;

        // Metadata used while executing (only for timed effects)
        bool isPaused;
        EffectResult lastExecutionResult;
    } Effect;

    // Only touched on the game thread
    std::vector<std::unique_ptr<Effect>> activeEffects;
    std::chrono::steady_clock::time_point lastActiveEffectsTick;
    // Timed effects left over from a disable, retried each frame until removal succeeds
    std::vector<std::unique_ptr<Effect>> effectsPendingRemoval;

    // Payloads received on the network thread, executed on the game thread
    std::queue<nlohmann::json> incomingPayloadQueue;
    std::mutex incomingPayloadQueueMutex;
    // Payloads emitted on the game thread, sent on the network thread
    std::queue<nlohmann::json> outgoingPayloadQueue;
    std::mutex outgoingPayloadQueueMutex;

    void HandleRemoteData(nlohmann::json payload);
    void ProcessIncomingPayload(nlohmann::json payload);
    void ClearPayloadQueues();
    bool TryRemoveEffect(Effect* effect);
    void ProcessPendingRemovals();
    void ProcessActiveEffects();

    void EmitMessage(uint32_t eventId, long timeRemaining, EffectResult status);
    void SendVersion(uint32_t id);
    std::unique_ptr<Effect> ParseMessage(nlohmann::json payload);
    EffectResult ExecuteEffect(Effect* effect);
    EffectResult CanApplyEffect(Effect* effect);
    EffectResult TranslateGiEnum(GameInteractionEffectQueryResult giResult);

  public:
    static CrowdControl* Instance;
    void Enable();
    void Disable();
    void OnIncomingJson(nlohmann::json payload);
    void OnConnected();
    void ProcessOutgoingPackets();
    void OnGameFrameUpdate();
    bool NeedsFrameUpdate();
};

#endif // __cplusplus
