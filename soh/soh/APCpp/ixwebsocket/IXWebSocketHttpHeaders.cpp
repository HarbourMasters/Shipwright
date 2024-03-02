/*
 *  IXWebSocketHttpHeaders.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2018 Machine Zone, Inc. All rights reserved.
 */

#include "IXWebSocketHttpHeaders.h"

#include "IXSocket.h"
#include <algorithm>
#include <locale>

namespace ix
{
    std::pair<bool, WebSocketHttpHeaders> parseHttpHeaders(
        std::unique_ptr<Socket>& socket, const CancellationRequest& isCancellationRequested)
    {
        WebSocketHttpHeaders headers;

        char line[1024];
        int i;

        while (true)
        {
            int colon = 0;

            for (i = 0; i < 2 || (i < 1023 && line[i - 2] != '\r' && line[i - 1] != '\n'); ++i)
            {
                if (!socket->readByte(line + i, isCancellationRequested))
                {
                    return std::make_pair(false, headers);
                }

                if (line[i] == ':' && colon == 0)
                {
                    colon = i;
                }
            }
            if (line[0] == '\r' && line[1] == '\n')
            {
                break;
            }

            // line is a single header entry. split by ':', and add it to our
            // header map. ignore lines with no colon.
            if (colon > 0)
            {
                line[i] = '\0';
                std::string lineStr(line);
                // colon is ':', usually colon+1 is ' ', and colon+2 is the start of the value.
                // some webservers do not put a space after the colon character, so
                // the start of the value might be farther than colon+2.
                // The spec says that space after the : should be discarded.
                // i is end of string (\0), i-colon is length of string minus key;
                // subtract 1 for '\0', 1 for '\n', 1 for '\r',
                // 1 for the ' ' after the ':', and total is -4
                // since we use an std::string later on and don't account for '\0',
                // plus the optional first space, total is -2
                int start = colon + 1;
                while (lineStr[start] == ' ')
                {
                    start++;
                }

                std::string name(lineStr.substr(0, colon));
                std::string value(lineStr.substr(start, lineStr.size() - start - 2));

                headers[name] = value;
            }
        }

        return std::make_pair(true, headers);
    }
} // namespace ix
