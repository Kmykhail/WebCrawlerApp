#pragma once

#ifndef CRAWLERMANAGER_H
#define CRAWLERMANAGER_H

#include "CrawlItem.h"
#include "UrlData.h"

#include <QNetworkAccessManager>
#include <QQueue>
#include <QThreadPool>
#include <QNetworkReply>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <chrono>
#include <gtest/gtest_prod.h>

using namespace std::chrono;

class CrawlerManager : public QObject
#include <QObject>
{
    Q_OBJECT
    Q_DISABLE_COPY(CrawlerManager);
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
    Q_PROPERTY(bool urlLimit READ getUrlLimit WRITE setUrlLimit NOTIFY urlLimitChanged)
    Q_PROPERTY(bool urlDepth READ getUrlDepth WRITE setUrlDepth NOTIFY urlDepthChanged)

    enum class ControlState {
        RUN, PAUSE, STOP
    };
    using enum ControlState;

public:
    explicit CrawlerManager(QObject *parent = nullptr);
    ~CrawlerManager();
    Q_INVOKABLE void start(const QString &url);
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    bool isRunning() const;

    qint32 getUrlLimit() const;
    void setUrlLimit(qint32 urlLimit);

    qint32 getUrlDepth() const;
    void setUrlDepth(qint32 urlDepth);

public slots:
    void onUrlsParsed(const QSet<CrawlItem> &crawledItems);

signals:
    void finished();
    void runningChanged();
    void urlsDiscovered(const QList<UrlData> &batch);
    void urlLimitChanged(const qint32 urlLimit);
    void urlDepthChanged(const qint32 urlDepth);

private:
    void loadHtml(const CrawlItem &crawlItem);
    void processQueue();
    void clearThreadPool();
    FRIEND_TEST(CrawlerManagerTest, CheckUrlQueue);

private:
    qint32 m_depth{0};
    qint32 m_limit{0};
    qint32 m_activeDownloads{0};
    ControlState m_controlState{STOP};

    QThreadPool m_threadPool;
    QQueue<CrawlItem> m_urlQueue;
    QSet<QString> m_visitedUrls;
    QSet<QNetworkReply*> m_activeReplies;

    QString m_header{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"};
    QNetworkAccessManager *m_networkAccessManager{nullptr};

    // measurements
    steady_clock::time_point m_startTime;
};


#endif // CRAWLERMANAGER_H
