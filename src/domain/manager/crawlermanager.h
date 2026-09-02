#pragma once

#ifndef CRAWLERMANAGER_H
#define CRAWLERMANAGER_H

#include "CrawlItem.h"
#include "UrlData.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QQueue>
#include <QThreadPool>
#include <QNetworkReply>
#include <QTimer>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <gtest/gtest_prod.h>

#include "urlfetcher.h"
#include "queuehandler.h"

using namespace std::chrono;

class CrawlerManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(CrawlerManager);
    Q_PROPERTY(ControlState controlState READ getControlState NOTIFY controlStateChanged)

public:
    enum class ControlState {
        RUN, PAUSE, RESUME, STOP, IDLE
    };
    Q_ENUM(ControlState)
    using enum ControlState;

    explicit CrawlerManager(QObject *parent = nullptr);
    ~CrawlerManager();
    Q_INVOKABLE void start(const QString &url);
    Q_INVOKABLE void pause();
    Q_INVOKABLE void resume();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setUrlDepth(qint32 depth);
    Q_INVOKABLE void setUrlLimit(qint32 limit);

    ControlState getControlState() const;

public slots:
    void onLinkScraping(const QSet<CrawlItem> &crawledItems);

signals:
    void controlStateChanged(ControlState state);
    void fetched(const QList<UrlData> &fetched);
    void queuedChanged(qsizetype queued);
    void urlsDiscovered(const QList<UrlData> &batch);
    void finished();
    void urlLimitChanged();
    void urlDepthChanged();

private:
    void processQueue();
    void clearThreadPool();

private:
    UrlFetcher *m_urlFetcher{nullptr};
    QueueHandler *m_queueHandler{nullptr};
    QTimer *m_queueUpdataTimer{nullptr};
    qint32 m_lastEmittedQueueSize{-1};

    QThreadPool m_threadPool;
    ControlState m_controlState{IDLE};
    QList<UrlData> m_pendingBatch;
};

#endif // CRAWLERMANAGER_H
