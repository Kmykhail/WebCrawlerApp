#pragma once

#ifndef URLFETCHER_H
#define URLFETCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QElapsedTimer>
#include <QTimer>

#include "CrawlItem.h"
#include "FetchResult.h"

class UrlFetcher : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(UrlFetcher)

public:
    explicit UrlFetcher(QObject *parent = nullptr);
    ~UrlFetcher();

    void executeNetworkRequest(const CrawlItem &crawlItem);
    void setCustomUserAgent(const QString &userAgent);
    qsizetype activeDownloads() const;
    void abortNetworkReplies();

signals:
    void fetched(const FetchResult &fetchResult);

private:
    void replyFinished(QNetworkReply *reply);
    void checkTimeout();

private:
    struct ActiveRequest{
        CrawlItem crawlItem;
        QElapsedTimer elapsedTimer;
    };

    QNetworkAccessManager *m_networkAccessManager{nullptr};
    QTimer *m_timeoutTimer{nullptr};
    QString m_header{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"};
    QMap<QNetworkReply*, ActiveRequest> m_activeReplies;
};

#endif // URLFETCHER_H
