#pragma once

#ifndef URLDATA_H
#define URLDATA_H

#include <QString>
#include <QMetaType>
#include <QTime>

#include "FetchResult.h"
#include "CrawlItem.h"

struct UrlData {
    QString url;
    QTime time;
    quint16 statusCode;
    qint32 depth;
    qsizetype htmlSize;
    bool isFetched{false};

    explicit UrlData(const FetchResult &fetchResult)
        : url(fetchResult.crawlItem.url.toString())
        , time(QTime::currentTime())
        , statusCode(fetchResult.statusCode)
        , depth(fetchResult.crawlItem.depth)
        , htmlSize(fetchResult.html.size())
        , isFetched(fetchResult.success)
    {}

    explicit UrlData(const CrawlItem &crawlItem)
        : url(crawlItem.url.toString())
        , time(QTime::currentTime())
        , statusCode(0)
        , depth(crawlItem.depth)
        , htmlSize(0)
        , isFetched(false)
    {}
};

Q_DECLARE_METATYPE(UrlData)

#endif // URLDATA_H
