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
        for (const auto &url : m_foundUrls) {
          qDebug() << QStringLiteral("#%1, url:%2").arg(cnt++).arg(url);
        }
#endif
        stop();
        clearThreadPool();
    });
}

CrawlerManager::~CrawlerManager()
{
    qDebug() << Q_FUNC_INFO;
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
    qDebug() << Q_FUNC_INFO;
    if (m_controlState == RUN) {
        m_controlState = PAUSE;
        emit controlStateChanged();
    }
}

void CrawlerManager::resume()
{
    qDebug() << Q_FUNC_INFO;
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
    m_foundUrls.clear();
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
    qDebug() << Q_FUNC_INFO;
    return m_discoveredUrlLimit;
}

void CrawlerManager::setUrlLimit(qint32 urlLimit)
{
    qDebug() << Q_FUNC_INFO;
    if (m_controlState != RUN && urlLimit != m_discoveredUrlLimit) {
        m_discoveredUrlLimit = urlLimit;
        emit urlLimitChanged(m_discoveredUrlLimit);
    }
}

qint32 CrawlerManager::getUrlDepth() const
{
    return m_depth;
}

void CrawlerManager::setUrlDepth(qint32 urlDepth)
{
    qDebug() << Q_FUNC_INFO;
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
        m_activeDownloads--;
        return;
    }

    QNetworkRequest request(crawlItem.url);
    request.setHeader(QNetworkRequest::UserAgentHeader, m_header);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    QNetworkReply *reply = m_networkAccessManager->get(request);
    if (!reply) {
        m_activeDownloads--;
        return;
    }
    m_activeReplies.insert(reply);

    QTimer *timer = new QTimer(reply);
    timer->setSingleShot(true);
    timer->start(TIMEOUT_REQUEST_MS);

    connect(timer, &QTimer::timeout, reply, [this, reply, crawlItem](){
        qWarning() << QStringLiteral("Url: %1, timeout: %2")
                          .arg(crawlItem.url.toString());

        emit urlFetched({crawlItem.url.toString(), QTime(), TIMEOUT_CODE, crawlItem.depth, 0, false});
        QMetaObject::invokeMethod(reply, [reply](){
            if (reply && !reply->isFinished()) {
                reply->abort();
            }
        }, Qt::QueuedConnection);
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

                if (m_foundUrls.size() < m_discoveredUrlLimit) {
                    Worker *worker = new Worker(crawlItem, html);
                    connect(worker, &Worker::urlParsed, this, &CrawlerManager::onUrlsParsed);
                    m_threadPool.start(worker);
                }
                processQueue();
                return;
            }
            /* TODO FIX:
            QHttpNetworkConnectionPrivate::_q_hostLookupFinished could not de-queue request, failed to report HostNotFoundError
            QHttpNetworkConnectionPrivate::_q_hostLookupFinished could not de-queue request, failed to report HostNotFoundError
            */
            default: // for the rest errors
                auto statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
                auto code = statusCode > 0 ? static_cast<quint16>(statusCode) : static_cast<quint16>(error);
                qWarning() << QStringLiteral("Url: %1, responded with error status: %2")
                                .arg(crawlItem.url.toString())
                                .arg(statusCode);

                emit urlFetched({crawlItem.url.toString(), QTime(), code, crawlItem.depth, 0, false});
                reply->deleteLater();
                processQueue();
                return;
          }
    });

    emit queuedChanged(m_urlQueue.size());
}

void CrawlerManager::onUrlsParsed(const QSet<CrawlItem> &crawledItems) {
    qDebug() << Q_FUNC_INFO;

    QList<UrlData> batch;
    batch.reserve(crawledItems.size());
    for (const auto &item : crawledItems) {
        if (m_foundUrls.size() >= m_discoveredUrlLimit) break;

        auto strUrl = item.url.toString();
        if (!m_foundUrls.contains(strUrl)) {
            batch.append({strUrl, QTime::currentTime(), 0, item.depth, 0});

            m_foundUrls.insert(strUrl);
            m_urlQueue.enqueue(item);
        }
    }

    if (!batch.isEmpty()) {
        batch.shrink_to_fit();
        emit urlsDiscovered(batch);
        emit queuedChanged(m_urlQueue.size());
    }

    processQueue();

    if (m_foundUrls.size() >= m_discoveredUrlLimit && m_urlQueue.isEmpty() && !m_activeDownloads) {
        emit finished();
    }
}
