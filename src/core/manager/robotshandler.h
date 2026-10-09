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

    bool isWaitingHostsEmpty() const;

signals:
    void filtered(const QSet<CrawlItem> &crawlItems);
    void requiredRobotTxt(const CrawlItem &crawlItem);
    void evaluateWaitingItemsByHost(const QString &parsedHost);

private:
    struct RobotsRule{
        QString path;
        bool isAllowed{false};
    };
    using RobotsRulesByAgent = QHash<QString, QList<RobotsRule>>;

    bool evaluateItemWithRules(const CrawlItem &crawlItem, const QList<RobotsRule> &rules);
    void parse(const QByteArray &content, RobotsRulesByAgent &rules);
    const QList<RobotsRule> *rulesForHost(const QString &host) const;

private:

    QHash<QString, RobotsRulesByAgent> m_robotsRules;
    QHash<QString, QSet<CrawlItem>> m_waitingItemsByHost;
    QString m_userAgent{"*"};
};
