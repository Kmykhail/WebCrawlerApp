#pragma once
#ifndef FETCHRESULT_H
#define FETCHRESULT_H

#include "CrawlItem.h"

struct FetchResult {
    CrawlItem crawlItem;
    QByteArray html;
    quint16 statusCode;
    bool success;
};

#endif // FETCHRESULT_H
