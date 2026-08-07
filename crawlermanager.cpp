#include "crawlermanager.h"

#include <QMap>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QThreadPool>
#include <qdebug.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstringliteral.h>
#include <qstringview.h>
#include <qtdeprecationdefinitions.h>
#include <qthread.h>
#include <qthreadpool.h>
#include <qtimer.h>
#include <qtmetamacros.h>
#include <qtypes.h>

namespace {
constexpr qint32 MAX_DEPTH = 20;
constexpr qint32 MAX_CONCURRENT_DOWNLOADS = 50;
} // namespace

class Worker : public QObject, public QRunnable {
  Q_OBJECT
public:
  Worker(const QByteArray &html, QObject *parent = nullptr)
      : m_html{html}, QObject{parent} {
    setAutoDelete(true);
  }
  void run() override {
    qDebug() << "Worker threda id: " << QThread::currentThreadId();
    if (m_html.isEmpty()) {
      qWarning() << "Empty html";
      emit urlParsed({});
      return;
    }

    QSet<QString> urls;
    for (const auto &match : m_regex.globalMatch(m_html)) {
      urls.insert(match.captured(1));
    }

    emit urlParsed(urls);
  }

signals:
  void urlParsed(const QSet<QString> &urls);

private:
  QByteArray m_html;
  CrawlerManager *m_crawlerManager{nullptr};
  QRegularExpression m_regex{"href=\"(https?://[^\"]+)\""};
};

CrawlerManager::CrawlerManager(QObject *parent) : QObject{parent} {
  m_networkAccessManager = new QNetworkAccessManager(this);
  m_depth = MAX_DEPTH;
  qDebug() << "maxThreadCount"
           << QThreadPool::globalInstance()->maxThreadCount();
}

void CrawlerManager::start(const QString &url) {
  m_urlQueue.enqueue(url);
  m_visitedUrls.insert(url);
  processQueue();
}

void CrawlerManager::processQueue() {
  while (!m_urlQueue.isEmpty() &&
         m_activeDownloads < MAX_CONCURRENT_DOWNLOADS) {
    QString url = m_urlQueue.dequeue();
    loadHtml(url);
  }

  if (m_urlQueue.isEmpty() && m_activeDownloads == 0) {
    for (const auto &url : m_visitedUrls) {
      qInfo() << url;
    }
    emit finished();
  }

  qDebug() << QStringLiteral("Queue size: %1, active loading %2/%3")
                  .arg(m_urlQueue.size())
                  .arg(m_activeDownloads)
                  .arg(MAX_CONCURRENT_DOWNLOADS);
}

void CrawlerManager::loadHtml(const QString &url) {
    m_activeDownloads++;
  QNetworkRequest request((QUrl(url)));
  request.setHeader(QNetworkRequest::UserAgentHeader, m_header);
  QNetworkReply *reply = m_networkAccessManager->get(request);
  connect(reply, &QNetworkReply::finished, this, [this, reply, url]() {
    if (auto error = reply->error();
        error != QNetworkReply::NetworkError::NoError) {
      qWarning() << "!NetworkError: " << error;
      m_activeDownloads--;
      reply->deleteLater();
      processQueue();
      return;
    }

    Worker *worker = new Worker(reply->readAll(), nullptr);
    connect(worker, &Worker::urlParsed, this, &CrawlerManager::onUrlsParsed,
            Qt::QueuedConnection);
    connect(
        worker, &Worker::destroyed, this,
        [this]() {
          m_activeDownloads--;
          processQueue();
        },
        Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(worker);
    reply->deleteLater();

    QTimer::singleShot(1000, this, [this]() { processQueue(); });
  });
}

void CrawlerManager::onUrlsParsed(const QSet<QString> &urls) {
  if (m_visitedUrls.size() >= m_depth) {
    qDebug() << QStringLiteral(
                    "Visited size reached limit| queue size: %1, limit: %2")
                    .arg(m_visitedUrls.size())
                    .arg(m_depth);
    processQueue();
    return;
  }

  for (const QString &url : urls) {
    if (!m_visitedUrls.contains(url)) {
      m_urlQueue.enqueue(url);
      m_visitedUrls.insert(url);
    }
  }
  // push the queue
  processQueue();
}

#include "crawlermanager.moc"
