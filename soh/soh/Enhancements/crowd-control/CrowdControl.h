#ifdef ENABLE_CROWD_CONTROL

#ifndef _CROWDCONTROL_C
#define _CROWDCONTROL_C
#endif

#include "stdint.h"

#ifdef __cplusplus
#include <SDL2/SDL_net.h>
#include <cstdint>
#include <thread>
#include <memory>
#include <map>
#include <vector>
#include <iostream>
#include <chrono>
#include <future>

class CrowdControl {
    private:
        enum EffectResult {
            /// <summary>The effect executed successfully.</summary>
            Success = 0x00,
            /// <summary>The effect failed to trigger, but is still available for use. Viewer(s) will be refunded. You probably don't want this.</summary>
            Failure = 0x01,
            /// <summary>Same as <see cref="Failure"/> but the effect is no longer available for use for the remainder of the game. You probably don't want this.</summary>
            Unavailable = 0x02,
            /// <summary>The effect cannot be triggered right now, try again in a few seconds. This is the "normal" failure response.</summary>
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
            /// <summary>Effect is being initiated. SoH exclusive to check against if an effect state has changed or not.</summary>
            Initiate = 0xFE,
            /// <summary>The processor isn't ready to start or has shut down.</summary>
            NotReady = 0xFF
        };

        enum ResponseType {
            EffectRequest = 0x00,
            Login = 0xF0,
            LoginSuccess = 0xF1,
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
            std::string type;
            uint32_t value;
            std::string category;
            long timeRemaining;

            // Metadata used while executing (only for timed effects)
            bool isPaused;
            EffectResult lastExecutionResult;
        } Effect;
        
        std::thread ccThreadReceive;
        std::thread ccThreadProcess;

        TCPsocket tcpsock;
        IPaddress ip;

        bool isEnabled;
        bool connected;

        char received[512];

        std::vector<Effect*> activeEffects;
        std::mutex activeEffectsMutex;

        void ListenToServer();
        void ProcessActiveEffects();

        void EmitMessage(TCPsocket socket, uint32_t eventId, long timeRemaining,
                                       CrowdControl::EffectResult status);
        Effect* ParseMessage(char payload[512]);
        EffectResult ExecuteEffect(std::string effectId, uint32_t value, bool dryRun);
        void RemoveEffect(std::string effectId);
        bool SpawnEnemy(std::string effectId);

    public:
        static CrowdControl* Instance;
        void Init();
        void Shutdown();
        void Enable();
        void Disable();
};
#endif
#endif
