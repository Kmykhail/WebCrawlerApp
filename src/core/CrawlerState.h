#pragma once
#ifndef CRAWLERSTATE_H
#define CRAWLERSTATE_H

#include <QMetaType>

struct CrawlerState {
    Q_GADGET
    Q_PROPERTY(bool running MEMBER running)
    Q_PROPERTY(int controlState MEMBER controlState)
    Q_PROPERTY(qint32 discovered MEMBER discovered)
    Q_PROPERTY(qint32 queued MEMBER queued)
    Q_PROPERTY(qint32 fetched MEMBER fetched)
    Q_PROPERTY(qint32 failed MEMBER failed)
public:
    bool running{false};
    int controlState{4}; // 4 = IDLE
    qint32 discovered{0};
    qint32 queued{0};
    qint32 fetched{0};
    qint32 failed{0};
};

#endif // CRAWLERSTATE_H
