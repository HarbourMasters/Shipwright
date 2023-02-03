#include "GameInteractor.h"
#include <spdlog/spdlog.h>

// MARK: - Remote

void GameInteractor::EnableRemoteInteractor(const char *host, Uint16 port) {
    if (isRemoteInteractorEnabled) {
        return;
    }

    if (SDLNet_ResolveHost(&remoteIP, host, port) == -1) {
        SPDLOG_ERROR("[GameInteractor] SDLNet_ResolveHost: {}", SDLNet_GetError());
    }

    isRemoteInteractorEnabled = true;
    remoteThreadReceive = std::thread(&GameInteractor::ReceiveFromServer, this);
}

void GameInteractor::RegisterRemoteForwarder(std::function<void(nlohmann::json)> method) {
    remoteForwarder = method;
}

void GameInteractor::DisableRemoteInteractor() {
    if (!isRemoteInteractorEnabled) {
        return;
    }

    isRemoteInteractorEnabled = false;
    remoteThreadReceive.join();
    remoteForwarder = nullptr;
}

void GameInteractor::TransmitMessageToRemote(nlohmann::json payload) {
    std::string jsonPayload = payload.dump();
    SDLNet_TCP_Send(remoteSocket, jsonPayload.c_str(), jsonPayload.size() + 1);
}

// MARK: - Private

void GameInteractor::ReceiveFromServer() {
    while (isRemoteInteractorEnabled) {
        while (!isRemoteInteractorConnected && isRemoteInteractorEnabled) {
            SPDLOG_TRACE("[GameInteractor] Attempting to make connection to server...");
            remoteSocket = SDLNet_TCP_Open(&remoteIP);

            if (remoteSocket) {
                isRemoteInteractorConnected = true;
                SPDLOG_TRACE("[GameInteractor] Connection to server established!");
                break;
            }
        }

        SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
        if (remoteSocket) {
            SDLNet_TCP_AddSocket(socketSet, remoteSocket);
        }

        // Listen to socket messages
        while (isRemoteInteractorConnected && remoteSocket && isRemoteInteractorEnabled) {
            // we check first if socket has data, to not block in the TCP_Recv
            int socketsReady = SDLNet_CheckSockets(socketSet, 0);

            if (socketsReady == -1) {
                SPDLOG_ERROR("[GameInteractor] SDLNet_CheckSockets: {}", SDLNet_GetError());
                break;
            }

            if (socketsReady == 0) {
                continue;
            }

            char remoteDataReceived[512];
            int len = SDLNet_TCP_Recv(remoteSocket, &remoteDataReceived, sizeof(remoteDataReceived));
            if (!len || !remoteSocket || len == -1) {
                SPDLOG_ERROR("[GameInteractor] SDLNet_TCP_Recv: {}", SDLNet_GetError());
                break;
            }

            HandleRemoteMessage(remoteDataReceived);
        }

        if (isRemoteInteractorConnected) {
            SDLNet_TCP_Close(remoteSocket);
            isRemoteInteractorConnected = false;
            SPDLOG_TRACE("[GameInteractor] Ending receiving thread...");
        }
    }
}

void GameInteractor::HandleRemoteMessage(char message[512]) {
    nlohmann::json payload = nlohmann::json::parse(message, nullptr, false);

    if (remoteForwarder) {
        remoteForwarder(payload);
        return;
    }

    // Handle events on our own
}
