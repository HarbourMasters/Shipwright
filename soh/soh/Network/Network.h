#ifndef NETWORK_H
#define NETWORK_H
#ifdef __cplusplus

#include <thread>
#include <string>

#ifdef __EMSCRIPTEN__
#include "WebSocket.h"
#elif defined(ENABLE_REMOTE_CONTROL)
#include <SDL2/SDL_net.h>
#endif

#include <nlohmann/json.hpp>

class Network {
  private:
#ifdef __EMSCRIPTEN__
    WebSocketClient wsClient;
    void PollWebSocket();
#elif defined(ENABLE_REMOTE_CONTROL)
    IPaddress networkAddress;
    TCPsocket networkSocket;
#endif
#ifndef __EMSCRIPTEN__
    std::thread receiveThread;
#endif
    std::string receivedData;

    void ReceiveFromServer();
    void HandleRemoteData(char payload[512]);
    void HandleRemoteJson(std::string payload);

  public:
    bool isEnabled;
    bool isConnected;

    void Enable(const char* host, uint16_t port);
    void Disable();
    virtual void OnIncomingData(char payload[512]);
    virtual void OnIncomingJson(nlohmann::json payload);
    virtual void OnConnected();
    virtual void OnDisconnected();
    virtual void ProcessOutgoingPackets();
    void SendDataToRemote(const char* payload);
    virtual void SendJsonToRemote(nlohmann::json packet);

#ifdef __EMSCRIPTEN__
    // WebSocket-specific: connect to a WebSocket URL
    void EnableWebSocket(const std::string& url);
    // Poll for incoming WebSocket messages (call from game thread)
    void PollIncoming();
#endif
};

#endif // __cplusplus
#endif // NETWORK_H
