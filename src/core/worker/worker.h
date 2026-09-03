#pragma once

#include "CrawlItem.h"
#include <QObject>
#include <QRunnable>
#include <QUrl>
#include <QRegularExpression>
#include <qtclasshelpermacros.h>

class Worker : public QObject, public QRunnable {
    Q_OBJECT
    Q_DISABLE_COPY(Worker);
public:
    Worker(const CrawlItem& crawlItem, const QByteArray &html, QObject *parent = nullptr);
    void run() override;

signals:
    void finished(const QSet<CrawlItem> &crawlItems);

private:
    const CrawlItem m_crawlItem;
    const QByteArray m_html;
    const QRegularExpression m_regex{"href=\"(https?://[^\"]+)\""};
};
