#include "crawlermanager.h"
#include "worker.h"

#include <QMap>
#include <qlogging.h>
#include <qobject.h>
#include <qstringliteral.h>

namespace {
constexpr qint32 MAX_URLS = 1000;
constexpr qint32 MAX_DEPTH = 5;
constexpr qint32 MAX_CONCURRENT_DOWNLOADS = 20;
} // namespace

CrawlerManager::CrawlerManager(QObject *parent) : QObject{parent} {
  m_networkAccessManager = new QNetworkAccessManager(this);
  m_depth = MAX_DEPTH;
  m_threadPool.setMaxThreadCount(QThread::idealThreadCount());

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
    onUrlsParsed({ CrawlItem{QUrl{url}, 0} });
}

void CrawlerManager::pause() {
    if (m_controlState == RUN) {
        m_controlState = PAUSE;
    }
}

void CrawlerManager::stop() {
    m_controlState = STOP;
    m_urlQueue.clear();
    m_visitedUrls.clear();

    for (auto it = m_activeReplies.begin(); it != m_activeReplies.end(); ++it) {
        (*it)->abort();
    }

    emit runningChanged();
}

bool CrawlerManager::isRunning() const {
  return m_controlState == RUN;
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
  if (m_controlState != RUN) return;

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
          if (m_controlState != RUN) return;

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
  if (m_visitedUrls.size() >= MAX_URLS) {
      emit finished();
      return;
  }

  for (const auto &item : crawledItems) {
    if (m_visitedUrls.size() >= MAX_URLS) break;

    auto strUrl = item.url.toString();
    if (!m_visitedUrls.contains(strUrl)) {
        m_visitedUrls.insert(strUrl);
        m_urlQueue.enqueue(item);

        emit newUrlFound(strUrl);
    }
  }

  processQueue();
}


