#pragma once

#ifndef CRAWLERMANAGER_H
#define CRAWLERMANAGER_H

#include "CrawlItem.h"

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
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
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

public slots:
    void onUrlsParsed(const QSet<CrawlItem> &crawledItems);

signals:
    void finished();
    void runningChanged();
    void newUrlFound(const QString &url);

private:
    void loadHtml(const CrawlItem &crawlItem);
    void processQueue();
    void clearThreadPool();
    FRIEND_TEST(CrawlerManagerTest, CheckUrlQueue);

private:
    int m_depth{0};
    int m_activeDownloads{0};
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
