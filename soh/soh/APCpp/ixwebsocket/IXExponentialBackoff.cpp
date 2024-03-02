/*
 *  IXExponentialBackoff.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXExponentialBackoff.h"

#include <cmath>

namespace ix
{
    uint32_t calculateRetryWaitMilliseconds(uint32_t retryCount,
                                            uint32_t maxWaitBetweenReconnectionRetries,
                                            uint32_t minWaitBetweenReconnectionRetries)
    {
        // It's easy with a power function to go beyond 2^32, and then 
        // have unexpected results, so prepare for that
        const uint32_t maxRetryCountWithoutOverflow = 26;

        uint32_t waitTime = 0;
        if (retryCount < maxRetryCountWithoutOverflow)
        {
            waitTime = std::pow(2, retryCount) * 100;
        }

        if (waitTime < minWaitBetweenReconnectionRetries)
        {
            waitTime = minWaitBetweenReconnectionRetries;
        }

        if (waitTime > maxWaitBetweenReconnectionRetries)
        {
            waitTime = maxWaitBetweenReconnectionRetries;
        }

        if (retryCount >= maxRetryCountWithoutOverflow)
        {
            waitTime = maxWaitBetweenReconnectionRetries;
        }

        return waitTime;
    }
} // namespace ix
