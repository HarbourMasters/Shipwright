/*
 *  IXExponentialBackoff.h
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2017-2019 Machine Zone, Inc. All rights reserved.
 */

#pragma once

#include <cstdint>

namespace ix
{
    uint32_t calculateRetryWaitMilliseconds(uint32_t retryCount,
                                            uint32_t maxWaitBetweenReconnectionRetries,
                                            uint32_t minWaitBetweenReconnectionRetries);
} // namespace ix
