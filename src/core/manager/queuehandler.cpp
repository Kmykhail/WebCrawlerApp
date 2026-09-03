#include "queuehandler.h"

namespace {
constexpr qint32 DEFAULT_LIMIT = 10000;
constexpr qint32 DEFAULT_DEPTH = 3;
}

QueueHandler::QueueHandler(QObject *parent)
    : QObject{parent}
{
    setUrlDepth(DEFAULT_DEPTH);
    setUrlLimit(DEFAULT_LIMIT);
}

void QueueHandler::enqueue(const QSet<CrawlItem> &crawledItems)
{
    QList<CrawlItem> batch;
    batch.reserve(crawledItems.size());

    for (const auto &item: crawledItems) {
        auto visitedUrlsSize = m_visitedUrls.size();
        if (visitedUrlsSize >= m_urlLimit) break;

        m_visitedUrls.insert(item.url);
        if (visitedUrlsSize != m_visitedUrls.size()) {
            batch.append(item);
            m_urlQueue.enqueue(item);
        }
    }

    if (!batch.isEmpty()) {
        batch.shrink_to_fit();
        emit urlsDiscovered(batch);
    }
}

[[nodiscard]]
std::optional<CrawlItem> QueueHandler::dequeue()
{
    if (!m_urlQueue.isEmpty()) {
        return m_urlQueue.dequeue();
    }
    return std::nullopt;
}

bool QueueHandler::isQueueEmpty() const
{
    return m_urlQueue.isEmpty();
}

qsizetype QueueHandler::queueSize() const
{
    return m_urlQueue.size();
}

void QueueHandler::clearQueue()
{
    m_urlQueue.clear();
}

qsizetype QueueHandler::visitedSize() const
{
    return m_visitedUrls.size();
}

void QueueHandler::clearVisited()
{
    m_visitedUrls.clear();
}

void QueueHandler::clearAll()
{
    clearQueue();
    clearVisited();
}

bool QueueHandler::isUnderLimit() const
{
    return m_visitedUrls.size() < m_urlLimit;
}

qint32 QueueHandler::getUrlLimit() const
{
    return m_urlLimit;
}

void QueueHandler::setUrlLimit(qint32 urlLimit)
{
    if (m_urlLimit != urlLimit) {
        m_urlLimit = urlLimit;
        emit urlLimitChanged();
    }
}

qint32 QueueHandler::getUrlDepth() const
{
    return m_depth;
}

void QueueHandler::setUrlDepth(qint32 urlDepth)
{
    if (m_depth != urlDepth) {
        m_depth = urlDepth;
        emit urlDepthChanged();
    }
}
