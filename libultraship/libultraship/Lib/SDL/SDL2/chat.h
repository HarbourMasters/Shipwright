/*
  CHAT:  A chat client/server using the SDL example network library
  Copyright (C) 1997-2016 Sam Lantinga <slouken@libsdl.org>

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

/* $Id$ */

/* Convert four letters into a number */
#define MAKE_NUM(A, B, C, D)    (((A+B)<<8)|(C+D))

/* Defines for the chat client */
#define CHAT_SCROLLBACK 512     /* Save 512 lines in scrollback */
#define CHAT_PROMPT "> "
#define CHAT_PACKETSIZE 256     /* Maximum length of a message */

/* Defines shared between the server and client */
#define CHAT_PORT   MAKE_NUM('C','H','A','T')

/* The protocol between the chat client and server */
#define CHAT_HELLO  0   /* 0+Port+len+name */
#define CHAT_HELLO_PORT     1
#define CHAT_HELLO_NLEN     CHAT_HELLO_PORT+2
#define CHAT_HELLO_NAME     CHAT_HELLO_NLEN+1
#define CHAT_ADD    1   /* 1+N+IP+Port+len+name */
#define CHAT_ADD_SLOT       1
#define CHAT_ADD_HOST       CHAT_ADD_SLOT+1
#define CHAT_ADD_PORT       CHAT_ADD_HOST+4
#define CHAT_ADD_NLEN       CHAT_ADD_PORT+2
#define CHAT_ADD_NAME       CHAT_ADD_NLEN+1
#define CHAT_DEL    2   /* 2+N */
#define CHAT_DEL_SLOT       1
#define CHAT_DEL_LEN        CHAT_DEL_SLOT+1
#define CHAT_BYE    255 /* 255 */
#define CHAT_BYE_LEN        1

/* The maximum number of people who can talk at once */
#define CHAT_MAXPEOPLE  10
