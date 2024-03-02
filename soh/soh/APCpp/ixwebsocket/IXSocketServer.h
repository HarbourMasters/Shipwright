/*
 *  IXSocketServer.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include "IXConnectionState.h"
#include "IXNetSystem.h"
#include "IXSelectInterrupt.h"
#include "IXSocketTLSOptions.h"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <utility> // pair

namespace ix
{
    class Socket;

    class SocketServer
    {
    public:
        using ConnectionStateFactory = std::function<std::shared_ptr<ConnectionState>()>;

        // Each connection is handled by its own worker thread.
        // We use a list as we only care about remove and append operations.
        using ConnectionThreads =
            std::list<std::pair<std::shared_ptr<ConnectionState>, std::thread>>;

        SocketServer(int port = SocketServer::kDefaultPort,
                     const std::string& host = SocketServer::kDefaultHost,
                     int backlog = SocketServer::kDefaultTcpBacklog,
                     size_t maxConnections = SocketServer::kDefaultMaxConnections,
                     int addressFamily = SocketServer::kDefaultAddressFamily);
        virtual ~SocketServer();
        virtual void stop();

        // It is possible to override ConnectionState through inheritance
        // this method allows user to change the factory by returning an object
        // that inherits from ConnectionState but has its own methods.
        void setConnectionStateFactory(const ConnectionStateFactory& connectionStateFactory);

        const static int kDefaultPort;
        const static std::string kDefaultHost;
        const static int kDefaultTcpBacklog;
        const static size_t kDefaultMaxConnections;
        const static int kDefaultAddressFamily;

        void start();
        std::pair<bool, std::string> listen();
        void wait();

        void setTLSOptions(const SocketTLSOptions& socketTLSOptions);

        int  getPort();
        std::string getHost();
        int getBacklog();
        std::size_t getMaxConnections();
        int getAddressFamily();
    protected:
        // Logging
        void logError(const std::string& str);
        void logInfo(const std::string& str);

        void stopAcceptingConnections();

    private:
        // Member variables
        int _port;
        std::string _host;
        int _backlog;
        size_t _maxConnections;
        int _addressFamily;

        // socket for accepting connections
        socket_t _serverFd;

        std::atomic<bool> _stop;

        std::mutex _logMutex;

        // background thread to wait for incoming connections
        std::thread _thread;
        void run();
        void onSetTerminatedCallback();

        // background thread to cleanup (join) terminated threads
        std::atomic<bool> _stopGc;
        std::thread _gcThread;
        void runGC();

        // the list of (connectionState, threads) for each connections
        ConnectionThreads _connectionsThreads;
        std::mutex _connectionsThreadsMutex;

        // used to have the main control thread for a server
        // wait for a 'terminate' notification without busy polling
        std::condition_variable _conditionVariable;
        std::mutex _conditionVariableMutex;

        // the factory to create ConnectionState objects
        ConnectionStateFactory _connectionStateFactory;

        virtual void handleConnection(std::unique_ptr<Socket>,
                                      std::shared_ptr<ConnectionState> connectionState) = 0;
        virtual size_t getConnectedClientsCount() = 0;

        // Returns true if all connection threads are joined
        void closeTerminatedThreads();
        size_t getConnectionsThreadsCount();

        SocketTLSOptions _socketTLSOptions;

        // to wake up from select
        SelectInterruptPtr _acceptSelectInterrupt;

        // used by the gc thread, to know that a thread needs to be garbage collected
        // as a connection
        std::condition_variable _conditionVariableGC;
        std::mutex _conditionVariableMutexGC;
        bool _canContinueGC{ false };
    };
} // namespace ix
