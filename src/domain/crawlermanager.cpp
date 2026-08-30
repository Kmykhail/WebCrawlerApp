#include "crawlermanager.h"
#include "worker.h"

#include <QMap>
#include <qlogging.h>
#include <qobject.h>
#include <qstringliteral.h>
#include <QTime>
#include <QTimer>

namespace {
constexpr qint32 DEFAULT_LIMIT = 10000;
constexpr qint32 DEFAULT_DEPTH = 3;
constexpr qint32 MAX_CONCURRENT_DOWNLOADS = 20;
constexpr qint32 FETCH_BATCH_THRESHOLD = 30;
constexpr qint32 TIMEOUT_REQUEST_MS = 5000;
constexpr quint16 TIMEOUT_CODE = 499;
} // namespace

CrawlerManager::CrawlerManager(QObject *parent) : QObject{parent} {
    m_networkAccessManager = new QNetworkAccessManager(this);
    m_threadPool.setMaxThreadCount(QThread::idealThreadCount());
    m_fetchBatch.reserve(FETCH_BATCH_THRESHOLD);
    setUrlLimit(DEFAULT_LIMIT);
    setUrlDepth(DEFAULT_DEPTH);

    connect(this, &CrawlerManager::finished, this, [this](){
#ifdef QT_DEBUG
        auto elapsed = duration_cast<std::chrono::milliseconds>(steady_clock::now() - m_startTime);
        qInfo() << QStringLiteral("elapsed milliseconds: %1").arg(elapsed.count());

        size_t cnt = 1;
        for (const auto &url : m_visitedUrls) {
          qDebug() << QStringLiteral("#%1, url:%2").arg(cnt++).arg(url);
        }
#endif
        stop();
        clearThreadPool();
    });
}

CrawlerManager::~CrawlerManager()
{
    if (m_controlState != STOP) {
        stop();
    }
    clearThreadPool();
}

void CrawlerManager::start(const QString &url) {
    qDebug() << Q_FUNC_INFO;
    if (url.isEmpty() || m_controlState == RUN) return;

#ifdef QT_DEBUG
    m_startTime = steady_clock::now();
#endif

    m_controlState = RUN;
    emit controlStateChanged();
    onUrlsParsed({ CrawlItem{QUrl{url}, 0} });
}

void CrawlerManager::pause() {
    if (m_controlState == RUN) {
        m_controlState = PAUSE;
        emit controlStateChanged();
    }
}

void CrawlerManager::resume()
{
    if (m_controlState == PAUSE) {
        m_controlState = RUN;
        emit controlStateChanged();
        processQueue();
    }
}

void CrawlerManager::stop() {
    qDebug() << Q_FUNC_INFO;
    m_controlState = STOP;
    m_urlQueue.clear();
    m_visitedUrls.clear();
    if (!m_fetchBatch.isEmpty()) {
        emit urlsFetched(m_fetchBatch);
        m_fetchBatch.clear();
    }

    const QSet<QNetworkReply*> repliesToAbort = m_activeReplies;
    for (auto *reply: repliesToAbort) {
        reply->abort();
        reply->deleteLater();
    }

    emit controlStateChanged();
}

qint32 CrawlerManager::getUrlLimit() const
{
    return m_limit;
}

void CrawlerManager::setUrlLimit(qint32 urlLimit)
{
    if (m_controlState != RUN && urlLimit != m_limit) {
        m_limit = urlLimit;
        emit urlLimitChanged(m_limit);
    }
}

qint32 CrawlerManager::getUrlDepth() const
{
    return m_depth;
}

void CrawlerManager::setUrlDepth(qint32 urlDepth)
{
    if (m_controlState != RUN && urlDepth != m_depth) {
        m_depth = urlDepth;
        emit urlDepthChanged(m_depth);
    }
}

CrawlerManager::ControlState CrawlerManager::getControlState() const
{
    return m_controlState;
}

void CrawlerManager::processQueue() {
    qDebug() << Q_FUNC_INFO;
    if (m_controlState != RUN) return;

    while (!m_urlQueue.isEmpty() && m_activeDownloads < MAX_CONCURRENT_DOWNLOADS) {
        auto item = m_urlQueue.dequeue();
        m_activeDownloads++;
        loadHtml(item);
    }

    qDebug() << "Queue:" << m_urlQueue.size()
             << "active downloads:" << m_activeDownloads
             << "/" << MAX_CONCURRENT_DOWNLOADS;
}

void CrawlerManager::clearThreadPool()
{
    qDebug() << Q_FUNC_INFO;
    m_threadPool.clear();
    m_threadPool.waitForDone();
}

void CrawlerManager::loadHtml(const CrawlItem &crawlItem) {
    qDebug() << Q_FUNC_INFO;
    if (m_controlState != RUN) {
        if (m_controlState == PAUSE) {
            m_urlQueue.prepend(crawlItem);
        }
        return;
    }

    QNetworkRequest request(crawlItem.url);
    request.setHeader(QNetworkRequest::UserAgentHeader, m_header);
    QNetworkReply *reply = m_networkAccessManager->get(request);
    m_activeReplies.insert(reply);

    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    timer->start(TIMEOUT_REQUEST_MS);

    connect(timer, &QTimer::timeout, reply, [this, reply, crawlItem](){
        if (m_fetchBatch.size() >= FETCH_BATCH_THRESHOLD) {
            emit urlsFetched(m_fetchBatch);
            m_fetchBatch.clear();
        }

        m_fetchBatch.append({crawlItem.url.toString(), QTime(), TIMEOUT_CODE, crawlItem.depth, 0});

        reply->abort();
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply, crawlItem]() {
        m_activeReplies.remove(reply);
        m_activeDownloads--;

        switch (auto error = reply->error(); error) {
            case QNetworkReply::OperationCanceledError: // ignore `abort` which will come form CrawlerManager::stop
                reply->deleteLater();
                processQueue();
                return;
            case QNetworkReply::NoError: // valid case
            {
                if (m_fetchBatch.size() >= FETCH_BATCH_THRESHOLD) {
                    emit urlsFetched(m_fetchBatch);
                    m_fetchBatch.clear();
                }

                auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                auto html = reply->readAll();
                reply->deleteLater();
                if (m_controlState != RUN) {
                    if (m_controlState == PAUSE) {
                      m_urlQueue.prepend(crawlItem);
                    }
                    return;
                }

                m_fetchBatch.append({crawlItem.url.toString(), QTime(), static_cast<quint16>(statusCode), crawlItem.depth, html.size()});

                Worker *worker = new Worker(crawlItem, html);
                connect(worker, &Worker::urlParsed, this, &CrawlerManager::onUrlsParsed);
                m_threadPool.start(worker);
                return;
            }
            default: // for the rest errors
                if (m_fetchBatch.size() >= FETCH_BATCH_THRESHOLD) {
                    emit urlsFetched(m_fetchBatch);
                    m_fetchBatch.clear();
                }

                auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                auto code = statusCode > 0 ? static_cast<quint16>(statusCode) : static_cast<quint16>(error);

                m_fetchBatch.append({crawlItem.url.toString(), QTime(), code, crawlItem.depth, 0});

                reply->deleteLater();
                processQueue();
                return;
          }
    });
}

void CrawlerManager::onUrlsParsed(const QSet<CrawlItem> &crawledItems) {
    qDebug() << Q_FUNC_INFO;
    if (m_visitedUrls.size() >= m_limit) {
        emit finished();
        return;
    }

    QList<UrlData> batch;
    batch.reserve(crawledItems.size());
    for (const auto &item : crawledItems) {
        if (m_visitedUrls.size() >= m_limit) break;

        auto strUrl = item.url.toString();
        if (!m_visitedUrls.contains(strUrl)) {
            batch.append({strUrl, QTime::currentTime(), 0, item.depth, 0});

            m_visitedUrls.insert(strUrl);
            m_urlQueue.enqueue(item);
        }
    }

    if (!batch.isEmpty()) {
        batch.shrink_to_fit();
        emit urlsDiscovered(batch);
    }

  processQueue();
}
