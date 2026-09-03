#ifndef QUEUEHANDLER_H
#define QUEUEHANDLER_H

#include <QObject>
#include <QUrl>
#include <QSet>
#include <QQueue>
#include <optional>

#include "CrawlItem.h"

class QueueHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QueueHandler)
    Q_PROPERTY(bool urlLimit READ getUrlLimit WRITE setUrlLimit NOTIFY urlLimitChanged)
    Q_PROPERTY(bool urlDepth READ getUrlDepth WRITE setUrlDepth NOTIFY urlDepthChanged)
public:
    explicit QueueHandler(QObject *parent = nullptr);

    void enqueue(const QSet<CrawlItem> &crawledItems);
    std::optional<CrawlItem> dequeue();
    bool isQueueEmpty() const;
    qsizetype queueSize() const;
    void clearQueue();

    qsizetype visitedSize() const;
    void clearVisited();
    void clearAll();

    bool isUnderLimit() const;

    qint32 getUrlLimit() const;
    void setUrlLimit(qint32 urlLimit);

    qint32 getUrlDepth() const;
    void setUrlDepth(qint32 urlDepth);

signals:
    void queuedChanged(qsizetype size);
    void urlsDiscovered(const QList<CrawlItem> &batch);
    void urlLimitChanged();
    void urlDepthChanged();

private:
    quint32 m_depth{0};
    quint32 m_urlLimit{0};
    QQueue<CrawlItem> m_urlQueue;
    QSet<QUrl> m_visitedUrls;
};

#endif // QUEUEHANDLER_H
