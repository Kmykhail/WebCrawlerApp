#pragma once
#ifndef CRAWLERSTATE_H
#define CRAWLERSTATE_H

#include "crawlermanager.h"

struct CrawlerState {
    using control_state_t = CrawlerManager::ControlState;

    Q_GADGET
    Q_PROPERTY(bool running MEMBER running)
    Q_PROPERTY(control_state_t controlState MEMBER controlState)
    Q_PROPERTY(qint32 discovered MEMBER discovered)
public:
    bool running{false};
    control_state_t controlState{control_state_t::STOP};
    qint32 discovered{0};
};

#endif // CRAWLERSTATE_H
