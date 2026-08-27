#include "crawlermanager.h"
#include "worker.h"

#include <QMap>
#include <qlogging.h>
#include <qobject.h>
#include <qstringliteral.h>
#include <QTime>

namespace {
constexpr qint32 DEFAULT_LIMIT = 10000;
constexpr qint32 DEFAULT_DEPTH = 3;
constexpr qint32 MAX_CONCURRENT_DOWNLOADS = 20;
} // namespace

CrawlerManager::CrawlerManager(QObject *parent) : QObject{parent} {
    m_networkAccessManager = new QNetworkAccessManager(this);
    m_threadPool.setMaxThreadCount(QThread::idealThreadCount());
    setUrlLimit(DEFAULT_LIMIT);
    setUrlDepth(DEFAULT_DEPTH);

    connect(this , &CrawlerManager::finished, this, [this](){
        auto elapsed = duration_cast<std::chrono::milliseconds>(steady_clock::now() - m_startTime);
        qInfo() << QStringLiteral("elapsed milliseconds: %1").arg(elapsed.count());

        size_t cnt = 1;
        for (const auto &url : m_visitedUrls) {
          qDebug() << QStringLiteral("#%1, url:%2").arg(cnt++).arg(url);
        }
        stop();
        clearThreadPool();
    });
}

CrawlerManager::~CrawlerManager()
{
    clearThreadPool();
}

void CrawlerManager::start(const QString &url) {
    qDebug() << Q_FUNC_INFO;
    if (url.isEmpty() || m_controlState == RUN) return;
    m_startTime = steady_clock::now();

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
    m_controlState = STOP;
    m_urlQueue.clear();
    m_visitedUrls.clear();

    for (auto it = m_activeReplies.begin(); it != m_activeReplies.end(); ++it) {
        (*it)->abort();
    }

    emit controlStateChanged();
}

bool CrawlerManager::isRunning() const {
  return m_controlState == RUN;
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
  connect(reply, &QNetworkReply::finished, this, [this, reply, crawlItem]() {
    m_activeReplies.remove(reply);
      m_activeDownloads--;

      switch (auto error = reply->error(); error) {
      case QNetworkReply::OperationCanceledError: // ignore `abort` which will come form CrawlerManager::stop
          reply->deleteLater();
          return;
      case QNetworkReply::NoError: // valid case
      {
          auto html = reply->readAll();
          reply->deleteLater();
          if (m_controlState != RUN) {
              if (m_controlState == PAUSE) {
                  m_urlQueue.prepend(crawlItem);
              }
              return;
          }

          Worker *worker = new Worker(crawlItem, html);
          connect(worker, &Worker::urlParsed, this, &CrawlerManager::onUrlsParsed);
          m_threadPool.start(worker);
          return;
      }
      default: // for the rest errors
          qWarning() << QStringLiteral("Network error: %1 for url: %2").arg(error).arg(crawlItem.url.toString());
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
            batch.append({strUrl, QTime::currentTime(), 200, item.depth});

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


