/*
 *  IXSelectInterrupt.cpp
 *  Author: Benjamin Sergeant
 *  Copyright (c) 2019 Machine Zone, Inc. All rights reserved.
 */

#include "IXSelectInterrupt.h"

namespace ix
{
    const uint64_t SelectInterrupt::kSendRequest = 1;
    const uint64_t SelectInterrupt::kCloseRequest = 2;

    SelectInterrupt::SelectInterrupt()
    {
        ;
    }

    SelectInterrupt::~SelectInterrupt()
    {
        ;
    }

    bool SelectInterrupt::init(std::string& /*errorMsg*/)
    {
        return true;
    }

    bool SelectInterrupt::notify(uint64_t /*value*/)
    {
        return true;
    }

    uint64_t SelectInterrupt::read()
    {
        return 0;
    }

    bool SelectInterrupt::clear()
    {
        return true;
    }

    int SelectInterrupt::getFd() const
    {
        return -1;
    }

    void* SelectInterrupt::getEvent() const
    {
        return nullptr;
    }
} // namespace ix
