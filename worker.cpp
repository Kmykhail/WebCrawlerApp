#include "worker.h"
#include <QThread>

Worker::Worker(const QString &url, const QByteArray &html, QObject *parent)
    : m_qUrl{url}
    , m_html{html}
    , QObject{parent} {
    setAutoDelete(true);
}
void Worker::run() {
    qDebug() << "Worker threda id: " << QThread::currentThreadId();
    if (m_html.isEmpty()) {
      qWarning() << "Empty html";
      return;
    }

    QSet<QString> urls;
    for (const auto &match : m_regex.globalMatch(m_html)) {
        auto rawUrl = match.captured(1);
        QUrl normalized = m_qUrl.resolved(QUrl(rawUrl));
        if (!normalized.isValid() || normalized.scheme().isEmpty() || normalized.host().isEmpty()) continue;

        normalized.setFragment({});
        normalized = normalized.adjusted(QUrl::NormalizePathSegments);
        const QString normalizedUrl = normalized.toString(QUrl::FullyEncoded);
        urls.insert(normalizedUrl);
    }

    emit urlParsed(urls);
}
