/*
  SDL_net:  An example cross-platform network library for use with SDL
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>
  Copyright (C) 2012 Simeon Maxein <smaxein@googlemail.com>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef SDL_NET_H_
#define SDL_NET_H_

#ifdef WITHOUT_SDL
#include <stdint.h>
typedef uint8_t Uint8;
typedef uint16_t Uint16;
typedef uint32_t Uint32;

typedef struct SDLNet_version {
    Uint8 major;
    Uint8 minor;
    Uint8 patch;
} SDLNet_version;

#else /* WITHOUT_SDL */

#if __APPLE__
#include <SDL.h>
#include "SDL_endian.h"
#include "SDL_version.h"
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_endian.h>
#include <SDL2/SDL_version.h>
#endif

typedef SDL_version SDLNet_version;

#endif /* WITHOUT_SDL */

#include "begin_code.h"

/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Printable format: "%d.%d.%d", MAJOR, MINOR, PATCHLEVEL
*/
#define SDL_NET_MAJOR_VERSION   2
#define SDL_NET_MINOR_VERSION   1
#define SDL_NET_PATCHLEVEL      0

/* This macro can be used to fill a version structure with the compile-time
 * version of the SDL_net library.
 */
#define SDL_NET_VERSION(X)                          \
{                                                   \
    (X)->major = SDL_NET_MAJOR_VERSION;             \
    (X)->minor = SDL_NET_MINOR_VERSION;             \
    (X)->patch = SDL_NET_PATCHLEVEL;                \
}

#if SDL_NET_MAJOR_VERSION < 3 && SDL_MAJOR_VERSION < 3
/**
 *  This is the version number macro for the current SDL_net version.
 *
 *  In versions higher than 2.9.0, the minor version overflows into
 *  the thousands digit: for example, 2.23.0 is encoded as 4300.
 *  This macro will not be available in SDL 3.x or SDL_net 3.x.
 *
 *  Deprecated, use SDL_NET_VERSION_ATLEAST or SDL_NET_VERSION instead.
 */
#define SDL_NET_COMPILEDVERSION \
    SDL_VERSIONNUM(SDL_NET_MAJOR_VERSION, SDL_NET_MINOR_VERSION, SDL_NET_PATCHLEVEL)
#endif /* SDL_NET_MAJOR_VERSION < 3 && SDL_MAJOR_VERSION < 3 */

/**
 *  This macro will evaluate to true if compiled with SDL_net at least X.Y.Z.
 */
#define SDL_NET_VERSION_ATLEAST(X, Y, Z) \
    ((SDL_NET_MAJOR_VERSION >= X) && \
     (SDL_NET_MAJOR_VERSION > X || SDL_NET_MINOR_VERSION >= Y) && \
     (SDL_NET_MAJOR_VERSION > X || SDL_NET_MINOR_VERSION > Y || SDL_NET_PATCHLEVEL >= Z))

/**
 * Query the version of SDL_net that the program is linked against.
 *
 * This function gets the version of the dynamically linked SDL_net library.
 * This is separate from the SDL_NET_VERSION() macro, which tells you what
 * version of the SDL_net headers you compiled against.
 *
 * This returns static internal data; do not free or modify it!
 *
 * \returns a pointer to the version information.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC const SDLNet_version * SDLCALL SDLNet_Linked_Version(void);

/**
 * Initialize SDL_net.
 *
 * You must successfully call this function before it is safe to call any
 * other function in this library, with one exception: a human-readable error
 * message can be retrieved from SDLNet_GetError() if this function fails.
 *
 * SDL must be initialized before calls to functions in this library, because
 * this library uses utility functions from the SDL library.
 *
 * It is safe to call this more than once; the library keeps a counter of init
 * calls, and decrements it on each call to SDLNet_Quit, so you must pair your
 * init and quit calls.
 *
 * \returns 0 on success, -1 on error.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC int SDLCALL SDLNet_Init(void);

/**
 * Deinitialize SDL_net.
 *
 * You must call this when done with the library, to free internal resources.
 * It is safe to call this when the library isn't initialized, as it will just
 * return immediately.
 *
 * Once you have as many quit calls as you have had successful calls to
 * SDLNet_Init, the library will actually deinitialize.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC void SDLCALL SDLNet_Quit(void);


/* IPv4 hostname resolution API... */

typedef struct {
    Uint32 host;            /* 32-bit IPv4 host address */
    Uint16 port;            /* 16-bit protocol port */
} IPaddress;

#ifndef INADDR_ANY
#define INADDR_ANY      0x00000000
#endif
#ifndef INADDR_NONE
#define INADDR_NONE     0xFFFFFFFF
#endif
#ifndef INADDR_LOOPBACK
#define INADDR_LOOPBACK     0x7f000001
#endif
#ifndef INADDR_BROADCAST
#define INADDR_BROADCAST    0xFFFFFFFF
#endif

/**
 * Resolve a host name and port to an IP address in network form.
 *
 * If `host` is NULL, the resolved host will be set to `INADDR_ANY`.
 *
 * If the host couldn't be resolved, the host portion of the returned address
 * will be INADDR_NONE, and the function will return -1.
 *
 * \param address to be filled in with the resolved address and port.
 * \param host the hostname to lookup (like "libsdl.org")
 * \param port the port intended to be connected to, to fill into address.
 * \returns zero on success, -1 on error.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC int SDLCALL SDLNet_ResolveHost(IPaddress *address, const char *host, Uint16 port);

/**
 * Resolve an IP address to a host name in canonical form.
 *
 * If the IP couldn't be resolved, this function returns NULL, otherwise a
 * pointer to a static buffer containing the hostname is returned.
 *
 * **Warning**: This function is not thread-safe!
 *
 * \param ip the IP address to resolve into a hostname.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC const char * SDLCALL SDLNet_ResolveIP(const IPaddress *ip);

/**
 * Get the addresses of network interfaces on this system.
 *
 * \param addresses where to store the returned information.
 * \param maxcount the number of results that can be stored at `addresses`
 * \returns the number of addresses saved in `addresses`
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC int SDLCALL SDLNet_GetLocalAddresses(IPaddress *addresses, int maxcount);


/* TCP network API */

typedef struct _TCPsocket *TCPsocket;

/**
 * Open a TCP network socket.
 *
 * If `ip->host` is INADDR_NONE or INADDR_ANY, this creates a local server
 * socket on the given port, otherwise a TCP connection to the remote host and
 * port is attempted. The address passed in should already be swapped to
 * network byte order (addresses returned from SDLNet_ResolveHost() are
 * already in the correct form).
 *
 * \param ip The address to open a connection to (or to host a server on).
 * \returns the newly created socket, or NULL if there was an error.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_TCP_Close
 */
extern DECLSPEC TCPsocket SDLCALL SDLNet_TCP_Open(IPaddress *ip);

/**
 * Accept an incoming connection on the given server socket.
 *
 * `server` must be a socket returned by SDLNet_TCP_Open with an address of
 * INADDR_NONE or INADDR_ANY (a "server socket"). Other sockets do not accept
 * connections.
 *
 * \param server the server socket to accept a connection on.
 * \returns the newly created socket, or NULL if there was an error.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC TCPsocket SDLCALL SDLNet_TCP_Accept(TCPsocket server);

/**
 * Get the IP address of the remote system associated with the socket.
 *
 * If the socket is a server socket, this function returns NULL.
 *
 * This returns a pointer to internal memory; you should not modify or free
 * it, and should assume it's only valid until the socket is given to
 * SDLNet_TCP_Close.
 *
 * \param sock the socket to query.
 * \returns the address information for the socket.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC IPaddress * SDLCALL SDLNet_TCP_GetPeerAddress(TCPsocket sock);

/**
 * Send data over a non-server socket.
 *
 * `sock` must be a valid socket that was created by SDLNet_TCP_Open with a
 * specific address, or SDLNet_TCP_Accept.
 *
 * This function sends `len` bytes, pointed to by `data` over the non-server
 * socket `sock`.
 *
 * This function returns the actual amount of data sent. If the return value
 * is less than the amount of data sent, then either the remote connection was
 * closed, or an unknown socket error occurred.
 *
 * This function may block!
 *
 * \param sock the socket to send data to.
 * \param data a pointer to the bytes to send.
 * \param len the number of bytes, pointed to by `data`, to transmit.
 * \returns number of bytes sent, which might be less if there was a problem
 *          or connection failure. If the socket is invalid, this function can
 *          return -1, but in valid uses it'll return >= 0.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_TCP_Recv
 */
extern DECLSPEC int SDLCALL SDLNet_TCP_Send(TCPsocket sock, const void *data, int len);

/**
 * Receive data from a non-server socket.
 *
 * `sock` must be a valid socket that was created by SDLNet_TCP_Open with a
 * specific address, or SDLNet_TCP_Accept.
 *
 * Receive up to `maxlen` bytes of data over the non-server socket `sock`, and
 * store them in the buffer pointed to by `data`.
 *
 * This function returns the actual amount of data received. If the return
 * value is less than or equal to zero, then either the remote connection was
 * closed, or an unknown socket error occurred.
 *
 * Note that this will return the number of bytes available at the first
 * moment the socket is able to see new data. If packets are coming in slowly
 * from the network, this might be less data than you expect at a given time.
 *
 * This function may block! Use SDLNet_CheckSockets() to make sure there is
 * data available before calling this function, if you want to avoid blocking.
 *
 * \param sock the socket to send data to.
 * \param data a pointer to where to store received data.
 * \param maxlen the maximum number of bytes that can be stored at `data`.
 * \returns number of bytes received, which might be less than `maxlen`.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_TCP_Send
 * \sa SDLNet_CheckSockets
 */
extern DECLSPEC int SDLCALL SDLNet_TCP_Recv(TCPsocket sock, void *data, int maxlen);

/**
 * Close a TCP network socket.
 *
 * All TCP sockets (server and non-server) are deinitialized through this
 * function. Call this once you are done with a socket, and assume the handle
 * is invalid once you have.
 *
 * Closing a socket will disconnect any communication and free its resources.
 *
 * \param sock socket to close.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC void SDLCALL SDLNet_TCP_Close(TCPsocket sock);


/* UDP network API */

/* The maximum channels on a a UDP socket */
#define SDLNET_MAX_UDPCHANNELS  32
/* The maximum addresses bound to a single UDP socket channel */
#define SDLNET_MAX_UDPADDRESSES 4

typedef struct _UDPsocket *UDPsocket;
typedef struct {
    int channel;        /* The src/dst channel of the packet */
    Uint8 *data;        /* The packet data */
    int len;            /* The length of the packet data */
    int maxlen;         /* The size of the data buffer */
    int status;         /* packet status after sending */
    IPaddress address;  /* The source/dest address of an incoming/outgoing packet */
} UDPpacket;

/**
 * Allocate a single UDP packet.
 *
 * This allocates a packet with `size` bytes of space for payload.
 *
 * When done with this packet, you can free it with SDLNet_FreePacket. Packets
 * can be reused multiple times; you don't have to allocate a new one for each
 * piece of data you intend to send.
 *
 * You can allocate multiple packets at once with SDLNet_AllocPacketV.
 *
 * \param size the maximum number of bytes of payload this packet will
 *             contain.
 * \returns the new packet, or NULL if the function ran out of memory.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_ResizePacket
 * \sa SDLNet_FreePacket
 * \sa SDLNet_AllocPacketV
 */
extern DECLSPEC UDPpacket * SDLCALL SDLNet_AllocPacket(int size);


/**
 * Reallocate a UDP packet's payload space.
 *
 * This takes an existing packet and makes sure it can contain at least
 * `newsize` bytes of space for payload.
 *
 * When done with this packet, you can free it with SDLNet_FreePacket. Packets
 * can be used multiple times; you don't have to allocate a new one for each
 * piece of data you intend to send.
 *
 * Please note that on memory allocation failure, this function will leave the
 * existing buffer alone, and _will return the original buffer size_. It will
 * not return an error value, it'll just leave the packet as it was!
 *
 * **Warning**: Existing contents of the packet's data are lost when resizing,
 * whether you are growing or shrinking the payload space, since SDL_net does
 * not realloc the existing data.
 *
 * \param newsize the new maximum number of bytes of payload this packet will
 *                contain.
 * \returns the new maximum payload size, which will be unchanged from the
 *          previous if the system ran out of memory.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_AllocPacket
 * \sa SDLNet_FreePacket
 */
extern DECLSPEC int SDLCALL SDLNet_ResizePacket(UDPpacket *packet, int newsize);


/**
 * Dispose of a UDP packet.
 *
 * This frees both the packet's payload and the packet itself. Once this call
 * completes, the packet's pointer is invalid and should not be used anymore.
 *
 * \param packet the packet to free.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_AllocPacket
 * \sa SDLNet_ResizePacket
 */
extern DECLSPEC void SDLCALL SDLNet_FreePacket(UDPpacket *packet);

/**
 * Allocate a UDP packet vector (array of packets).
 *
 * This allocates `howmany` packets at once, each `size` bytes long.
 *
 * You must free the results of this function with SDLNet_FreePacketV, and
 * must not free individual packets from this function with SDLNet_FreePacket.
 *
 * \param howmany the number of packets to allocate.
 * \param size the maximum bytes of payload each packet should contain.
 * \returns a pointer to the first packet in the array, or NULL if the
 *          function ran out of memory.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_FreePacketV
 */
extern DECLSPEC UDPpacket ** SDLCALL SDLNet_AllocPacketV(int howmany, int size);


/**
 * Free a UDP packet vector (array of packets).
 *
 * This frees the results of a previous call to SDLNet_AllocPacketV(), freeing
 * both the set of packets and the array that holds them.
 *
 * It is safe to free a NULL array through here; it's a harmless no-op.
 *
 * You must not use this to free packets allocated through any function other
 * than SDLNet_AllocPacketV().
 *
 * \param packetV the results of a call to SDLNet_AllocPacketV().
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_AllocPacketV
 */
extern DECLSPEC void SDLCALL SDLNet_FreePacketV(UDPpacket **packetV);

/**
 * Open a UDP network socket.
 *
 * If `port` is non-zero, the UDP socket is bound to a local port.
 *
 * The `port` should be given in native byte order, but is used internally in
 * network (big endian) byte order, in addresses, etc. This allows other
 * systems to send to this socket via a known port.
 *
 * Note that UDP sockets at the platform layer "bind" to a nework port number,
 * but SDL_net's UDP sockets also "bind" to a "channel" on top of that, with
 * SDLNet_UDP_Bind(). But the term is used for both.
 *
 * When you are done communicating over the returned socket, you can shut it
 * down and free its resources with SDLNet_UDP_Close().
 *
 * \param port the UDP port to bind this socket to.
 * \returns a new UDP socket, ready to communicate.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_UDP_Close
 * \sa SDLNet_UDP_Bind
 */
extern DECLSPEC UDPsocket SDLCALL SDLNet_UDP_Open(Uint16 port);

/**
 * Set the percentage of simulated packet loss for packets sent on the socket.
 *
 * SDL_net can optionally, at random, drop packets that are being sent and
 * received, to simulate bad networking conditions. As these sort of
 * conditions can happen in the real world but likely won't between machines
 * on the same LAN, you can use this function in testing to make sure your app
 * is robust against network problems even on a fast, reliable network.
 *
 * You probably don't want to use this function outside of local testing.
 *
 * \param sock the socket to simulate packet loss on.
 * \param percent a value from 0 to 100 of likelihood to drop a packet (higher
 *                the number means more likelihood of dropping.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC void SDLCALL SDLNet_UDP_SetPacketLoss(UDPsocket sock, int percent);

/**
 * Bind an address to the requested channel on the UDP socket.
 *
 * Note that UDP sockets at the platform layer "bind" to a nework port number,
 * but SDL_net's UDP sockets also "bind" to a "channel" on top of that, with
 * SDLNet_UDP_Bind(). But the term is used for both.
 *
 * If `channel` is -1, then the first unbound channel that has not yet been
 * bound to the maximum number of addresses will be bound with the given
 * address as it's primary address.
 *
 * If the channel is already bound, this new address will be added to the list
 * of valid source addresses for packets arriving on the channel. If the
 * channel is not already bound, then the address becomes the primary address,
 * to which all outbound packets on the channel are sent.
 *
 * \param sock the UDP socket to bind an address to a channel on.
 * \param channel the channel of the socket to bind to, or -1 to use the first
 *                available channel.
 * \param address the address to bind to the socket's channel.
 * \returns the channel which was bound, or -1 on error.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_UDP_Unbind
 */
extern DECLSPEC int SDLCALL SDLNet_UDP_Bind(UDPsocket sock, int channel, const IPaddress *address);

/**
 * Unbind all addresses from the given channel.
 *
 * Note that UDP sockets at the platform layer "bind" to a nework port number,
 * but SDL_net's UDP sockets also "bind" to a "channel" on top of that, with
 * SDLNet_UDP_Bind(). But the term is used for both.
 *
 * \param sock the UDP socket to unbind addresses from a channel on.
 * \param channel the channel of the socket to unbind.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_UDP_Bind
 */
extern DECLSPEC void SDLCALL SDLNet_UDP_Unbind(UDPsocket sock, int channel);

/**
 * Get the IP address of the remote system for a socket and channel.
 *
 * If `channel` is -1, then the primary IP port of the UDP socket is returned
 * -- this is only meaningful for sockets opened with a specific port.
 *
 * If the channel is not bound and not -1, this function returns NULL.
 *
 * \param sock the UDP socket to unbind addresses from a channel on.
 * \param channel the channel of the socket to unbind.
 * \returns the address bound to the socket's channel, or
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC IPaddress * SDLCALL SDLNet_UDP_GetPeerAddress(UDPsocket sock, int channel);

/**
 * Send a vector of packets to the the channels specified within the packet.
 *
 * If the channel specified in the packet is -1, the packet will be sent to
 * the address in the `src` member of the packet.
 *
 * Each packet will be updated with the status of the packet after it has been
 * sent, -1 if the packet send failed.
 *
 * This function takes an array of packets but does not need to be allocated
 * through SDLNet_AllocPacketV; if you supply your own array of packets you
 * allocated individually, that is okay.
 *
 * **Warning**: UDP is an _unreliable protocol_, which means we can report
 * that your packet has been successfully sent from your machine, but then it
 * never makes it to its destination when a router along the way quietly drops
 * it. If this happens--and this is a common result on the internet!--you will
 * not know the packet never made it. Also, packets may arrive in a different
 * order than you sent them. Plan accordingly!
 *
 * **Warning**: The maximum size of the packet is limited by the MTU (Maximum
 * Transfer Unit) of the transport medium. It can be as low as 250 bytes for
 * some PPP links, and as high as 1500 bytes for ethernet. Different sizes can
 * be sent, but the system might split it into multiple transmission fragments
 * behind the scenes, that need to be reassembled on the other side (and the
 * packet is lost if any fragment is lost in transit). So the less you can
 * reasonably send in a single packet, the better, as it will be more reliable
 * and lower latency.
 *
 * \param sock the UDP socket to send packets on.
 * \param packets an array of packets to send to the network.
 * \param npackets the number of packets in the `packets` array.
 * \returns the number of packets successfully sent from this machine.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_UDP_RecV
 */
extern DECLSPEC int SDLCALL SDLNet_UDP_SendV(UDPsocket sock, UDPpacket **packets, int npackets);

/**
 * Send a single UDP packet to the specified channel.
 *
 * If the channel specified is -1, the packet will be sent to the address in
 * the `src` member of the packet.
 *
 * The packet will be updated with the status of the packet after it has been
 * sent.
 *
 * **Warning**: UDP is an _unreliable protocol_, which means we can report
 * that your packet has been successfully sent from your machine, but then it
 * never makes it to its destination when a router along the way quietly drops
 * it. If this happens--and this is a common result on the internet!--you will
 * not know the packet never made it. Also, packets may arrive in a different
 * order than you sent them. Plan accordingly!
 *
 * **Warning**: The maximum size of the packet is limited by the MTU (Maximum
 * Transfer Unit) of the transport medium. It can be as low as 250 bytes for
 * some PPP links, and as high as 1500 bytes for ethernet. Different sizes can
 * be sent, but the system might split it into multiple transmission fragments
 * behind the scenes, that need to be reassembled on the other side (and the
 * packet is lost if any fragment is lost in transit). So the less you can
 * reasonably send in a single packet, the better, as it will be more reliable
 * and lower latency.
 *
 * \param sock the UDP socket to send packets on.
 * \param packet a single packet to send to the network.
 * \returns 1 if the packet was sent, or 0 on error.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC int SDLCALL SDLNet_UDP_Send(UDPsocket sock, int channel, UDPpacket *packet);

/**
 * Receive a vector of pending packets from a UDP socket.
 *
 * The returned packets contain the source address and the channel they
 * arrived on. If they did not arrive on a bound channel, the the channel will
 * be set to -1.
 *
 * The channels are checked in highest to lowest order, so if an address is
 * bound to multiple channels, the highest channel with the source address
 * bound will be returned.
 *
 * This function takes an array of packets but does not need to be allocated
 * through SDLNet_AllocPacketV; if you supply your own array of packets you
 * allocated individually, that is okay, as long as the last element in the
 * array is NULL, so SDL_net knows the array bounds. The arrays returned by
 * SDLNet_AllocPacketV are properly NULL-terminated for these purposes.
 *
 * This function does not block, so it can return 0 packets pending, which is
 * not an error condition.
 *
 * \param sock the UDP socket to receive packets on.
 * \param packets an array of packets, NULL terminated.
 * \returns the number of packets read from the network, or -1 on error. 0
 *          means no packets were currently available.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_UDP_SendV
 * \sa SDLNet_UDP_Recv
 */
extern DECLSPEC int SDLCALL SDLNet_UDP_RecvV(UDPsocket sock, UDPpacket **packets);

/**
 * Receive a single packet from a UDP socket.
 *
 * The returned packets contain the source address and the channel they
 * arrived on. If they did not arrive on a bound channel, the the channel will
 * be set to -1.
 *
 * The channels are checked in highest to lowest order, so if an address is
 * bound to multiple channels, the highest channel with the source address
 * bound will be returned.
 *
 * This function does not block, so it can return 0 packets pending, which is
 * not an error condition.
 *
 * \param sock the UDP socket to receive packets on.
 * \param packet a single packet to receive data into from the network.
 * \returns 1 if a new packet is available, or -1 on error. 0 means no packets
 *          were currently available.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_UDP_Send
 * \sa SDLNet_UDP_RecvV
 */
extern DECLSPEC int SDLCALL SDLNet_UDP_Recv(UDPsocket sock, UDPpacket *packet);

/**
 * Close a UDP socket.
 *
 * This disconnects the socket and frees any resources it retains.
 *
 * This socket may not be used again once given to this function.
 *
 * \param sock UDP socket to close.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC void SDLCALL SDLNet_UDP_Close(UDPsocket sock);


/***********************************************************************/
/* Hooks for checking sockets for available data                       */
/***********************************************************************/

typedef struct _SDLNet_SocketSet *SDLNet_SocketSet;

/* Any network socket can be safely cast to this socket type */
typedef struct _SDLNet_GenericSocket {
    int ready;
} *SDLNet_GenericSocket;

/**
 * Allocate a socket set for use with SDLNet_CheckSockets().
 *
 * To query if new data is available on a socket, you use a "socket set" with
 * SDLNet_CheckSockets(). A socket set is just a list of sockets behind the
 * scenes; you allocate a set and then add/remove individual sockets to/from
 * the set.
 *
 * When done with a socket set, you can free it with SDLNet_FreeSocketSet.
 *
 * \param maxsockets the maximum amount of sockets to include in this set.
 * \returns a socket set for up to `maxsockets` sockets, or NULL if the
 *          function ran out of memory.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_FreeSocketSet
 */
extern DECLSPEC SDLNet_SocketSet SDLCALL SDLNet_AllocSocketSet(int maxsockets);

/**
 * Add a socket to a socket set, to be checked for available data.
 *
 * Generally you don't want to call this generic function, but rather the
 * specific, inline function that wraps it: SDLNet_TCP_AddSocket() or
 * SDLNet_UDP_AddSocket().
 *
 * This function will fail if you add a socket to the set when the set already
 * has its maximum number of sockets added, but otherwise it will always
 * succeed.
 *
 * If `sock` is NULL, nothing is added to the set; this lets you query the
 * number of sockets currently contained in the set.
 *
 * \param set the socket set to add a new socket to.
 * \param sock the socket to add to the set.
 * \returns the total number of sockets contained in the set (including this
 *          new one), or -1 if the set is already full.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_TCP_AddSocket
 * \sa SDLNet_UDP_AddSocket
 * \sa SDLNet_DelSocket
 * \sa SDLNet_TCP_DelSocket
 * \sa SDLNet_UDP_DelSocket
 * \sa SDLNet_CheckSockets
 */
extern DECLSPEC int SDLCALL SDLNet_AddSocket(SDLNet_SocketSet set, SDLNet_GenericSocket sock);

/**
 * Add a TCP socket to a socket set, to be checked for available data.
 *
 * This is a small TCP-specific wrapper over SDLNet_AddSocket; please refer
 * to that function's documentation.
 *
 * \param set the socket set to add a new socket to.
 * \param sock the socket to add to the set.
 * \returns the total number of sockets contained in the set (including this new one),
 *          or -1 if the set is already full.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_AddSocket
 */
SDL_FORCE_INLINE int SDLNet_TCP_AddSocket(SDLNet_SocketSet set, TCPsocket sock)
{
    return SDLNet_AddSocket(set, (SDLNet_GenericSocket)sock);
}

/**
 * Add a UDP socket to a socket set, to be checked for available data.
 *
 * This is a small UDP-specific wrapper over SDLNet_AddSocket; please refer
 * to that function's documentation.
 *
 * \param set the socket set to add a new socket to.
 * \param sock the socket to add to the set.
 * \returns the total number of sockets contained in the set (including this new one),
 *          or -1 if the set is already full.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_AddSocket
 */
SDL_FORCE_INLINE int SDLNet_UDP_AddSocket(SDLNet_SocketSet set, UDPsocket sock)
{
    return SDLNet_AddSocket(set, (SDLNet_GenericSocket)sock);
}


/**
 * Remove a socket from a set of sockets to be checked for available data.
 *
 * Generally you don't want to call this generic function, but rather the
 * specific, inline function that wraps it: SDLNet_TCP_DelSocket() or
 * SDLNet_UDP_DelSocket().
 *
 * If `sock` is NULL, nothing is removed from the set; this lets you query the
 * number of sockets currently contained in the set.
 *
 * This will return -1 if the socket was not found in the set; in such a case,
 * nothing is removed from the set.
 *
 * \param set the socket set to remove a socket from.
 * \param sock the socket to remove from the set.
 * \returns the total number of sockets contained in the set (after `sock`'s
 *          removal), or -1 if `sock` was not in the set.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_TCP_DelSocket
 * \sa SDLNet_UDP_DelSocket
 * \sa SDLNet_AddSocket
 * \sa SDLNet_TCP_AddSocket
 * \sa SDLNet_UDP_AddSocket
 * \sa SDLNet_CheckSockets
 */
extern DECLSPEC int SDLCALL SDLNet_DelSocket(SDLNet_SocketSet set, SDLNet_GenericSocket sock);

/**
 * Remove a TCP socket from a socket set.
 *
 * This is a small TCP-specific wrapper over SDLNet_DelSocket; please refer
 * to that function's documentation.
 *
 * \param set the socket set to remove a socket from.
 * \param sock the socket to remove from the set.
 * \returns the total number of sockets contained in the set (after
 *          `sock`'s removal), or -1 if `sock` was not in the set.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_DelSocket
 */
SDL_FORCE_INLINE int SDLNet_TCP_DelSocket(SDLNet_SocketSet set, TCPsocket sock)
{
    return SDLNet_DelSocket(set, (SDLNet_GenericSocket)sock);
}

/**
 * Remove a UDP socket from a socket set.
 *
 * This is a small UDP-specific wrapper over SDLNet_DelSocket; please refer
 * to that function's documentation.
 *
 * \param set the socket set to remove a socket from.
 * \param sock the socket to remove from the set.
 * \returns the total number of sockets contained in the set (after
 *          `sock`'s removal), or -1 if `sock` was not in the set.
 *
 * \since This function is available since SDL_net 2.0.0.
 *
 * \sa SDLNet_DelSocket
 */
SDL_FORCE_INLINE int SDLNet_UDP_DelSocket(SDLNet_SocketSet set, UDPsocket sock)
{
    return SDLNet_DelSocket(set, (SDLNet_GenericSocket)sock);
}

/**
 * Check a socket set for data availability.
 *
 * This function checks to see if data is available for reading on the given
 * set of sockets. If 'timeout' is 0, it performs a quick poll, otherwise the
 * function returns when either data is available for reading, or the timeout
 * in milliseconds has elapsed, whichever occurs first.
 *
 * \param set the socket set to check for ready sockets.
 * \param timeout the time to wait in milliseconds for new data to arrive. A
 *                timeout of zero checks for new data and returns without
 *                blocking.
 * \returns the number of sockets ready for reading, or -1 if there was an
 *          error with the select() system call.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC int SDLCALL SDLNet_CheckSockets(SDLNet_SocketSet set, Uint32 timeout);

/* !!! FIXME: wikiheaders.pl ignores macros, atm */
/**
 * See if a specific socket has data available after checking it in a set.
 *
 * After calling SDLNet_CheckSockets(), you can use this function on a
 * socket that was in the socket set, to find out if data is available
 * for reading.
 *
 * \param sock the socket to check.
 * \returns non-zero if socket has new data available, zero otherwise.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
#define SDLNet_SocketReady(sock) _SDLNet_SocketReady((SDLNet_GenericSocket)(sock))
SDL_FORCE_INLINE int _SDLNet_SocketReady(SDLNet_GenericSocket sock)
{
    return (sock != NULL) && (sock->ready);
}

/**
 * Free a set of sockets allocated by SDLNet_AllocSocketSet().
 *
 * When done with a socket set, call this function to free its resources.
 *
 * This only frees the socket set, not the individual sockets in the set,
 * which would still (at some future point) need to be closed with
 * SDLNet_TCP_Close or SDLNet_UDP_Close.
 *
 * \param set the socket set to free.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC void SDLCALL SDLNet_FreeSocketSet(SDLNet_SocketSet set);


/* Error reporting functions */

/**
 * Set an error message to be retrieved with SDLNet_GetError.
 *
 * Generally you don't need to call this (SDL_net will use it internally to
 * report errors), but it could be useful if you need to inject an error
 * message of your own in here.
 *
 * \param fmt a printf-style format string for the error message.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC void SDLCALL SDLNet_SetError(const char *fmt, ...);

/**
 * Get the latest error message from SDL_net.
 *
 * The error message, depending on how SDL_net was built, may or may not be
 * thread-local data. Sometimes things will set an error message when no
 * failure was reported; the error string is only meaningful right after a
 * public API reports a failure, and should be ignored otherwise.
 *
 * \returns the last set error message in UTF-8 encoding.
 *
 * \since This function is available since SDL_net 2.0.0.
 */
extern DECLSPEC const char * SDLCALL SDLNet_GetError(void);

/* Inline functions to read/write network data */

/* Warning, most systems have data access alignment restrictions */
#if defined(__i386__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_IX86) || defined(_M_AMD64)
#define SDL_DATA_ALIGNED    0
#endif
#ifndef SDL_DATA_ALIGNED
#define SDL_DATA_ALIGNED    1
#endif

/* Write a 16/32-bit value to network packet buffer */
#define SDLNet_Write16(value, areap) _SDLNet_Write16(value, areap)
#define SDLNet_Write32(value, areap) _SDLNet_Write32(value, areap)

/* Read a 16/32-bit value from network packet buffer */
#define SDLNet_Read16(areap) _SDLNet_Read16(areap)
#define SDLNet_Read32(areap) _SDLNet_Read32(areap)

#if !defined(WITHOUT_SDL) && !SDL_DATA_ALIGNED

SDL_FORCE_INLINE void _SDLNet_Write16(Uint16 value, void *areap)
{
    *(Uint16 *)areap = SDL_SwapBE16(value);
}

SDL_FORCE_INLINE void _SDLNet_Write32(Uint32 value, void *areap)
{
    *(Uint32 *)areap = SDL_SwapBE32(value);
}

SDL_FORCE_INLINE Uint16 _SDLNet_Read16(const void *areap)
{
    return SDL_SwapBE16(*(const Uint16 *)areap);
}

SDL_FORCE_INLINE Uint32 _SDLNet_Read32(const void *areap)
{
    return SDL_SwapBE32(*(const Uint32 *)areap);
}

#else /* !defined(WITHOUT_SDL) && !SDL_DATA_ALIGNED */

SDL_FORCE_INLINE void _SDLNet_Write16(Uint16 value, void *areap)
{
    Uint8 *area = (Uint8*)areap;
    area[0] = (value >>  8) & 0xFF;
    area[1] =  value        & 0xFF;
}

SDL_FORCE_INLINE void _SDLNet_Write32(Uint32 value, void *areap)
{
    Uint8 *area = (Uint8*)areap;
    area[0] = (value >> 24) & 0xFF;
    area[1] = (value >> 16) & 0xFF;
    area[2] = (value >>  8) & 0xFF;
    area[3] =  value        & 0xFF;
}

SDL_FORCE_INLINE Uint16 _SDLNet_Read16(const void *areap)
{
    const Uint8 *area = (const Uint8*)areap;
    return ((Uint16)area[0]) << 8 | ((Uint16)area[1]);
}

SDL_FORCE_INLINE Uint32 _SDLNet_Read32(const void *areap)
{
    const Uint8 *area = (const Uint8*)areap;
    return ((Uint32)area[0]) << 24 | ((Uint32)area[1]) << 16 | ((Uint32)area[2]) << 8 | ((Uint32)area[3]);
}

#endif /* !defined(WITHOUT_SDL) && !SDL_DATA_ALIGNED */

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* SDL_NET_H_ */
