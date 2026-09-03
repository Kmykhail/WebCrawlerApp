#include "crawlermanager.h"
#include "worker.h"
#include "FetchResult.h"

#include <QMap>
#include <qlogging.h>
#include <qobject.h>
#include <qstringliteral.h>
#include <QTime>
#include <QTimer>
#include <ranges>

namespace {
constexpr qsizetype MAX_CONCURRENT_DOWNLOADS = 20;
constexpr qsizetype FETCH_BATCH_THRESHOLD = 30;
constexpr qsizetype QUEUE_UPDATE_INTERVAL_MS = 150;
} // namespace

CrawlerManager::CrawlerManager(QObject *parent)
    : QObject{parent}
    , m_urlFetcher{new UrlFetcher(this)}
    , m_queueHandler{new QueueHandler(this)}
{
    m_threadPool.setMaxThreadCount(QThread::idealThreadCount());

    m_queueUpdataTimer = new QTimer{this};
    m_queueUpdataTimer->setInterval(QUEUE_UPDATE_INTERVAL_MS);
    connect(m_queueUpdataTimer, &QTimer::timeout, this, [this](){
        auto currentQueueSize = m_queueHandler->queueSize();
        if (currentQueueSize != m_lastEmittedQueueSize) {
            m_lastEmittedQueueSize = currentQueueSize;
            emit queuedChanged(currentQueueSize);
        }
    });
    m_queueUpdataTimer->start();

    connect(m_queueHandler, &QueueHandler::urlLimitChanged, this, &CrawlerManager::urlLimitChanged);
    connect(m_queueHandler, &QueueHandler::urlDepthChanged, this, &CrawlerManager::urlDepthChanged);

    connect(m_urlFetcher, &UrlFetcher::fetched, this, [this](const FetchResult &fetchResult) {
        if (m_pendingBatch.size() >= FETCH_BATCH_THRESHOLD) {
            emit fetched(m_pendingBatch);
            m_pendingBatch.clear();
        }

        m_pendingBatch.append(UrlData{fetchResult});

        if (m_queueHandler->isUnderLimit()) {
            auto worker = new Worker(fetchResult.crawlItem, fetchResult.html);
            connect(worker, &Worker::finished, this, &CrawlerManager::onLinkScraping);
            m_threadPool.start(worker);
        }

        if (m_controlState == RUN) {
            processQueue();
        }
    });
    connect(m_queueHandler, &QueueHandler::urlsDiscovered, this, [this](const QList<CrawlItem> &batch) {
        auto urlDataList = batch
                           | std::ranges::views::transform([](const CrawlItem &item){ return UrlData{item}; })
                           | std::ranges::to<QList<UrlData>>();
        emit urlsDiscovered(urlDataList);
    });
    connect(this, &CrawlerManager::controlStateChanged, this, [this](ControlState state) {
        if (state == RUN || state == RESUME) {
            processQueue();
        }
    });
    connect(this, &CrawlerManager::finished, this, [this]() {
        qDebug() << "Crawling process completed";

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

void CrawlerManager::start(const QString &url)
{
    qDebug() << Q_FUNC_INFO;
    if (url.isEmpty() || m_controlState == RUN) return;

    m_controlState = RUN;
    m_queueHandler->enqueue({ CrawlItem{QUrl{url}, 0} });

    emit controlStateChanged(m_controlState);
}

void CrawlerManager::pause() {
    qDebug() << Q_FUNC_INFO;
    if (m_controlState == RUN) {
        m_controlState = PAUSE;
        emit controlStateChanged(m_controlState);
    }
}

void CrawlerManager::resume()
{
    qDebug() << Q_FUNC_INFO;
    if (m_controlState == PAUSE) {
        m_controlState = RUN;
        emit controlStateChanged(m_controlState);
    }
}

void CrawlerManager::stop()
{
    qDebug() << Q_FUNC_INFO;
    m_controlState = STOP;

    m_queueHandler->clearAll();
    m_urlFetcher->abortNetworkReplies();
    m_pendingBatch.clear();

    emit controlStateChanged(m_controlState);
}

void CrawlerManager::setUrlDepth(qint32 depth)
{
    qDebug() << Q_FUNC_INFO;
    if (m_controlState != RUN) {
        m_queueHandler->setUrlDepth(depth);
    }
}

void CrawlerManager::setUrlLimit(qint32 limit)
{
    qDebug() << Q_FUNC_INFO;
    if (m_controlState != RUN) {
        m_queueHandler->setUrlLimit(limit);
    }
}

void CrawlerManager::processQueue()
{
    qDebug() << Q_FUNC_INFO;

    while (!m_queueHandler->isQueueEmpty() && m_urlFetcher->activeDownloads() < MAX_CONCURRENT_DOWNLOADS) {
        if (m_controlState != RUN) break;
        std::optional<CrawlItem> item = m_queueHandler->dequeue();
        m_urlFetcher->executeNetworkRequest(item.value());
    }

    qDebug() << "Queue:" << m_queueHandler->queueSize()
             << "active downloads:" << m_urlFetcher->activeDownloads()
             << "/" << MAX_CONCURRENT_DOWNLOADS;

    if (!m_queueHandler->isUnderLimit() &&
        !m_urlFetcher->activeDownloads() &&
        m_queueHandler->isQueueEmpty()) {
        emit finished();
    }
}

void CrawlerManager::onLinkScraping(const QSet<CrawlItem> &crawledItems)
{
    qDebug() << Q_FUNC_INFO;

    m_queueHandler->enqueue(crawledItems);

    processQueue();
}

CrawlerManager::ControlState CrawlerManager::getControlState() const
{
    return m_controlState;
}

void CrawlerManager::clearThreadPool()
{
    qDebug() << Q_FUNC_INFO;
    m_threadPool.clear();
    m_threadPool.waitForDone();
}
