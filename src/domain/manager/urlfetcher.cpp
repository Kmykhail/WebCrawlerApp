#include <QTimer>
#include <QNetworkRequest>
#include <ranges>

#include "urlfetcher.h"

namespace{
constexpr qint32 TIMEOUT_REQUEST_MS = 5000;
constexpr qint32 TIMEOUT_CHECK_INTERVAL_MS = 500;
constexpr quint16 TIMEOUT_CODE = 499;
}

UrlFetcher::UrlFetcher(QObject *parent)
    : QObject{parent}
    , m_networkAccessManager{new QNetworkAccessManager(this)}
    , m_timeoutTimer{new QTimer(this)}
{
    connect(m_timeoutTimer, &QTimer::timeout, this, &UrlFetcher::checkTimeout);
    m_timeoutTimer->setInterval(TIMEOUT_CHECK_INTERVAL_MS);
    m_timeoutTimer->start();
}

UrlFetcher::~UrlFetcher()
{
    abortNetworkReplies();
}

void UrlFetcher::executeNetworkRequest(const CrawlItem &crawlItem)
{
    QNetworkRequest request(crawlItem.url);
    request.setHeader(QNetworkRequest::UserAgentHeader, m_header);
    request.setAttribute(QNetworkRequest::Http2AllowedAttribute, false);
    QNetworkReply *reply = m_networkAccessManager->get(request);
    if (!reply) return;

    ActiveRequest actiteReq;
    actiteReq.crawlItem = crawlItem;
    actiteReq.elapsedTimer.start();
    m_activeReplies.insert(reply, actiteReq);

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        replyFinished(reply);
    });
}

void UrlFetcher::replyFinished(QNetworkReply *reply)
{
    if (!m_activeReplies.contains(reply)) return;

    auto crawlItem = m_activeReplies.take(reply).crawlItem;

    switch (auto error = reply->error(); error) { // ignore `abort`
        case QNetworkReply::OperationCanceledError:
            reply->deleteLater();
            break;
        case QNetworkReply::NoError: // valid case
        {
            auto statusCode = static_cast<quint16>(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            auto html = reply->readAll();
            reply->deleteLater();

            emit fetched({crawlItem, html, statusCode, true});
            break;
        }
        default: // for the rest errors
            auto statusCode = static_cast<quint16>(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
            qWarning() << QStringLiteral("Url: %1, responded with error status: %2")
                              .arg(crawlItem.url.toString())
                              .arg(statusCode);

            emit fetched({crawlItem, {}, statusCode, false});
            reply->deleteLater();
            break;
    }
}

void UrlFetcher::checkTimeout()
{
    if (m_activeReplies.isEmpty()) return;

    QList<QNetworkReply*> timeoutReplies;
    for (auto it = m_activeReplies.begin(); it != m_activeReplies.end(); ++it) {
        if (it.value().elapsedTimer.hasExpired(TIMEOUT_REQUEST_MS)) {
            timeoutReplies.append(it.key());
        }
    }

    for (auto *reply: timeoutReplies) {
        auto req = m_activeReplies.take(reply);
        qWarning() << QStringLiteral("Url: %1, timeout").arg(req.crawlItem.url.toString());
        emit fetched({req.crawlItem, {}, TIMEOUT_CODE, false});

        if (reply && !reply->isFinished()) {
            reply->abort();
        }

        reply->deleteLater();
    }
}

void UrlFetcher::setCustomUserAgent(const QString &userAgent)
{
    m_header = userAgent;
}

[[nodiscard]]
qsizetype UrlFetcher::activeDownloads() const
{
    return m_activeReplies.size();
}

void UrlFetcher::abortNetworkReplies()
{
    if (m_activeReplies.isEmpty()) return;

    auto repliesToAbort = m_activeReplies.keys();
    for (auto *reply: repliesToAbort) {
        if (reply && !reply->isFinished()) {
            reply->abort();
        }
        reply->deleteLater();
    }
}
