#include "robotshandler.h"

#include <QTextStream>
#include <QDebug>

RobotsHandler::RobotsHandler(QObject *parent)
    : QObject{parent}
{
    connect(this, &RobotsHandler::evaluateWaitingItemsByHost, this, [this](const QString &host){
        if (auto it = m_waitingItemsByHost.find(host); it != m_waitingItemsByHost.end()) {
            QSet<CrawlItem> waitingItems = std::move(it.value());
            m_waitingItemsByHost.erase(it);

            auto ruleIt = m_robotsRules.find(m_userAgent);
            const auto &rules = ruleIt != m_robotsRules.end() ? ruleIt.value() : m_robotsRules.value("*");

            waitingItems.removeIf([this, &rules] (auto &item) {
                return !evaluateItemWithRules(item, rules);
            });

            emit filtered(waitingItems);
        }
    });
}

void RobotsHandler::evaluateUrl(const CrawlItem &crawlItem)
{
    qDebug() << Q_FUNC_INFO;

    evaluateUrls({crawlItem});
}

void RobotsHandler::evaluateUrls(const QSet<CrawlItem> &crawlItems)
{
    qDebug() << Q_FUNC_INFO;
    qDebug() << QStringLiteral("CrawlItems, size: %1").arg(crawlItems.size());

    QHash<QString, QSet<CrawlItem>> itemsByHost;
    for (const auto &item : crawlItems) {
        itemsByHost[item.url.host()].insert(item);
        qDebug() << QStringLiteral("host: %1, url: %2")
                       .arg(item.url.host())
                       .arg(item.url.toString());
    }

    for (auto it = itemsByHost.begin(); it != itemsByHost.end(); ++it) {
        const auto &host = it.key();
        QSet<CrawlItem> hostItems = std::move(it.value());

        if (m_waitingItemsByHost.contains(host)) {
            m_waitingItemsByHost[host].unite(hostItems);
            continue;
        }

        if (m_parsedHosts.contains(host) && !m_robotsRules.isEmpty()) {
            auto ruleIt = m_robotsRules.find(m_userAgent);
            const auto &rules = ruleIt != m_robotsRules.end() ? ruleIt.value() : m_robotsRules.value("*");

            hostItems.removeIf([this, &rules](auto &item) {return !evaluateItemWithRules(item, rules);});
            emit filtered(hostItems);
            continue;
        }

        qDebug() << QStringLiteral("Robots.txt file will be loaded for an unknown host: %1")
                        .arg(host);
        m_waitingItemsByHost[host].unite(hostItems);
        emit requiredRobotTxt(*(hostItems.begin()));
    }
}

void RobotsHandler::parseRobotsTxt(const FetchResult &respond)
{
    qDebug() << Q_FUNC_INFO;

    const auto &[item, html, statusCode, isSuccess] = respond;
    const auto &host = item.url.host();

    if (!isSuccess || html.isEmpty()) {
        if (!isSuccess) {
            qWarning() << QStringLiteral("Failed to download robots.txt, url: %1, status: %2")
                .arg(item.url.toString())
                .arg(statusCode);
        } else {
            qWarning() << QStringLiteral("Robots.txt for %1 empty").arg(item.url.toString());
        }

        if (auto it = m_waitingItemsByHost.find(host); it != m_waitingItemsByHost.end()) {
            auto waitingItems = std::move(it.value());

            qWarning() << QStringLiteral("Since robots.txt for host %1 failed, %2 urls are fallback to ALLOW")
                                .arg(host)
                                .arg(waitingItems.size());
            m_waitingItemsByHost.erase(it);
            m_parsedHosts.insert(host);
            emit filtered(waitingItems);
        }
        return;
    }

    QStringList currentUserAgents;
    QTextStream stream(html);
    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("#")) continue;

        int commentIndex = line.indexOf("#");
        if (commentIndex != -1) {
            line = line.left(commentIndex).trimmed();
        }

        int colonIndex = line.indexOf(":");
        if (colonIndex == -1) continue;
        auto key = line.left(colonIndex).trimmed().toLower();
        auto value = line.mid(colonIndex + 1).trimmed();

        if (key == "user-agent") {
            currentUserAgents += value;
        } else if (key == "allow") {
            for (const auto &userAgent : currentUserAgents) {
                m_robotsRules[userAgent].append({value, true});
            }
        } else if (key == "disallow") {
            for (const auto &userAgent : currentUserAgents) {
                m_robotsRules[userAgent].append({value, false});
            }
        }
    }
    m_parsedHosts.insert(host);
    emit evaluateWaitingItemsByHost(host);
}

void RobotsHandler::clear()
{
    qDebug() << Q_FUNC_INFO;

    m_robotsRules.clear();
    m_waitingItemsByHost.clear();
    m_parsedHosts.clear();
}

bool RobotsHandler::isWaitingHostsEmpty() const
{
    return m_waitingItemsByHost.isEmpty();
}

[[nodiscard]]
bool RobotsHandler::evaluateItemWithRules(const CrawlItem &crawlItem, const QList<RobotsRule> &rules)
{
    qDebug() << Q_FUNC_INFO;

    if (rules.empty()) {
        return true;
    }

    QString path = crawlItem.url.path();
    if (path.isEmpty()) {
        path = "/";
    }

    int longestMatchLength = -1;
    bool isAllowed = true;

    for (const auto &rule: rules) {
        bool matches = false;
        QString effectivePath = rule.path;

        if (effectivePath.endsWith('$')) {
            effectivePath.chop(1);
            matches = path == effectivePath;
        } else {
            matches = path.startsWith(effectivePath);
        }


        if (matches && effectivePath.length() > longestMatchLength) {
            longestMatchLength = effectivePath.length();
            isAllowed = rule.isAllowed;
        }
    }

    if (!isAllowed) {
        qWarning() << QStringLiteral("URL skipped: %1 is disallowed by robots.txt").arg(crawlItem.url.toString());
    }
    return isAllowed;
}

void RobotsHandler::setUserAgent(const QString &userAgent)
{
    m_userAgent = userAgent;
}
