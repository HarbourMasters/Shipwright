/*
 *  IXSocket.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2018 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>

#ifdef _WIN32
#include <basetsd.h>
#ifdef _MSC_VER
typedef SSIZE_T ssize_t;
#endif
#endif

#include "IXCancellationRequest.h"
#include "IXProgressCallback.h"
#include "IXSelectInterrupt.h"

namespace ix
{
    enum class PollResultType
    {
        ReadyForRead = 0,
        ReadyForWrite = 1,
        Timeout = 2,
        Error = 3,
        SendRequest = 4,
        CloseRequest = 5
    };

    class Socket
    {
    public:
        Socket(int fd = -1);
        virtual ~Socket();
        bool init(std::string& errorMsg);

        // Functions to check whether there is activity on the socket
        PollResultType poll(int timeoutMs = kDefaultPollTimeout);
        bool wakeUpFromPoll(uint64_t wakeUpCode);
        bool isWakeUpFromPollSupported();

        PollResultType isReadyToWrite(int timeoutMs);
        PollResultType isReadyToRead(int timeoutMs);

        // Virtual methods
        virtual bool accept(std::string& errMsg);

        virtual bool connect(const std::string& host,
                             int port,
                             std::string& errMsg,
                             const CancellationRequest& isCancellationRequested);
        virtual void close();

        virtual ssize_t send(char* buffer, size_t length);
        ssize_t send(const std::string& buffer);
        virtual ssize_t recv(void* buffer, size_t length);

        // Blocking and cancellable versions, working with socket that can be set
        // to non blocking mode. Used during HTTP upgrade.
        bool readByte(void* buffer, const CancellationRequest& isCancellationRequested);
        bool writeBytes(const std::string& str, const CancellationRequest& isCancellationRequested);

        std::pair<bool, std::string> readLine(const CancellationRequest& isCancellationRequested);
        std::pair<bool, std::string> readBytes(size_t length,
                                               const OnProgressCallback& onProgressCallback,
                                               const OnChunkCallback& onChunkCallback,
                                               const CancellationRequest& isCancellationRequested);

        static int getErrno();
        static bool isWaitNeeded();
        static void closeSocket(int fd);

        static PollResultType poll(bool readyToRead,
                                   int timeoutMs,
                                   int sockfd,
                                   const SelectInterruptPtr& selectInterrupt);

    protected:
        std::atomic<int> _sockfd;
        std::mutex _socketMutex;

        static bool readSelectInterruptRequest(const SelectInterruptPtr& selectInterrupt,
                                               PollResultType* pollResult);

    private:
        static const int kDefaultPollTimeout;
        static const int kDefaultPollNoTimeout;

        SelectInterruptPtr _selectInterrupt;
    };
} // namespace ix
