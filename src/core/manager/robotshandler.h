#pragma once

#include <QObject>
#include <QUrl>
#include <QByteArray>
#include <QHash>
#include <QSet>

#include "CrawlItem.h"
#include "FetchResult.h"

class RobotsHandler : public QObject
{
    Q_OBJECT
public:
    explicit RobotsHandler(QObject *parent = nullptr);

    void evaluateUrl(const CrawlItem &crawlItem);
    void evaluateUrls(const QSet<CrawlItem> &crawlItems);
    void setUserAgent(const QString &userAgent);
    void parseRobotsTxt(const FetchResult &respond);
    void clear();

signals:
    void filtered(const QSet<CrawlItem> &crawlItems);
    void requiredRobotTxt(const CrawlItem &crawlItem);
    void evaluateWaitingItemsByHost(const QString &parsedHost);

private:
    struct RobotsRule{
        QString path;
        bool isAllowed{false};
    };

    void evaluateItemWithRules(const CrawlItem &crawlItem, const QList<RobotsRule> &rules);

private:
    QHash<QString, QList<RobotsRule>> m_robotsRules;
    QHash<QString, QSet<CrawlItem>> m_waitingItemsByHost;
    QSet<QString> m_parsedHosts;
    QString m_userAgent{"*"};
};
