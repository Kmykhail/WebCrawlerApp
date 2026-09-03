#include "worker.h"
#include <QThread>

Worker::Worker(const CrawlItem &crawlItem, const QByteArray &html, QObject *parent)
    : m_crawlItem{crawlItem}
    , m_html{html}
    , QObject{parent} {
    setAutoDelete(true);
}
void Worker::run() {
    if (m_html.isEmpty()) {
      qWarning() << "Empty html";
      return;
    }

    QSet<CrawlItem> items;
    for (const auto &match : m_regex.globalMatch(m_html)) {
        auto rawUrl = match.captured(1);
        QUrl normalized = m_crawlItem.url.resolved(QUrl(rawUrl));

        if (!normalized.isValid() || normalized.scheme().isEmpty() || normalized.host().isEmpty()) continue;

        normalized.setFragment({});
        normalized = normalized.adjusted(QUrl::NormalizePathSegments);
        const QString normalizedUrl = normalized.toString(QUrl::FullyEncoded);
        items.insert({normalizedUrl, m_crawlItem.depth + 1});
    }

    emit finished(items);
}
