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
        typedef struct CCPacket {
            uint32_t packetId;
            std::string effectType;
            uint32_t effectValue;
            std::string effectCategory;
            long timeRemaining;
        } CCPacket;

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
        
        std::thread ccThreadReceive;

        TCPsocket tcpsock;
        IPaddress ip;

        bool connected;

        char received[512];

        std::vector<CCPacket*> receivedCommands;
        std::mutex receivedCommandsMutex;

        void RunCrowdControl(CCPacket* packet);
        void ReceiveFromCrowdControl();
        EffectResult ExecuteEffect(std::string effectId, uint32_t value, bool dryRun);
        bool SpawnEnemy(std::string effectId);
        void RemoveEffect(std::string effectId);

    public:
        static CrowdControl* Instance;
        void InitCrowdControl();
};
#endif
#endif
