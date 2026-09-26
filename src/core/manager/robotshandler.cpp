#include "robotshandler.h"

#include <QTextStream>
#include <QDebug>

RobotsHandler::RobotsHandler(QObject *parent)
    : QObject{parent}
{
    connect(this, &RobotsHandler::evaluateWaitingItemsByHost, this, [this](const QString &host){
        if (auto it = m_waitingItemsByHost.constFind(host); it != m_waitingItemsByHost.constEnd()) {
            auto waitingItems = std::move(it.value());
            m_waitingItemsByHost.erase(it);
            evaluateUrls(waitingItems);
        }
    });
}

void RobotsHandler::evaluateUrl(const CrawlItem &crawlItem)
{
    evaluateUrls({crawlItem});
}

void RobotsHandler::evaluateUrls(const QSet<CrawlItem> &crawlItems)
{
    QHash<QString, QSet<CrawlItem>> itemsByHost;
    for (const auto &item : crawlItems) {
        itemsByHost[item.url.host()].insert(item);
    }

    for (auto it = itemsByHost.constBegin(); it != itemsByHost.constEnd(); ++it) {
        const auto &host = it.key();
        const auto &hostItems = it.value();
        qDebug() << QStringLiteral("Host: %1, hostItems size: %2").arg(host).arg(hostItems.size());

        if (m_robotsRules.isEmpty() ||
            !m_parsedHosts.contains(host) ||
            m_waitingItemsByHost.contains(host)) {

            bool wasEmpty = !m_waitingItemsByHost.contains(host);
            m_waitingItemsByHost[host].unite(hostItems);

            qDebug() << "wasEmpty" << wasEmpty << ", m_parsedHosts.contains(host):" << m_parsedHosts.contains(host);
            if (wasEmpty && !m_parsedHosts.contains(host)) {
                qDebug() << "EMIT requiredRobotTxt";
                emit requiredRobotTxt(*(hostItems.begin()));
            }
            continue;
        }

        const QList<RobotsRule> &rules = m_robotsRules.contains(m_userAgent)
                                               ? m_robotsRules[m_userAgent]
                                               : m_robotsRules.value("*");

        for (const auto &item: hostItems) {
            evaluateItemWithRules(item, rules);
        }
    }
}

void RobotsHandler::parseRobotsTxt(const FetchResult &respond)
{
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

        if (auto it = m_waitingItemsByHost.constFind(item.url.host()); it != m_waitingItemsByHost.constEnd()) {
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
    m_robotsRules.clear();
    m_waitingItemsByHost.clear();
    m_parsedHosts.clear();
}

void RobotsHandler::evaluateItemWithRules(const CrawlItem &crawlItem, const QList<RobotsRule> &rules)
{
    QString path = crawlItem.url.path();
    if (path.isEmpty()) {
        path = "/";
    }

    if (rules.empty()) {
        emit filtered({crawlItem});
        return;
    }

    int longestMatchLength = -1;
    bool isAllowed = true;

    for (const auto &rule: rules) {
        if (path.startsWith(rule.path) &&
            rule.path.length() > longestMatchLength) {
            longestMatchLength = rule.path.length();
            isAllowed = rule.isAllowed;
        }
    }

    if (isAllowed) {
        emit filtered({crawlItem});
    } else {
        qWarning() << QStringLiteral("URL skipped: %1 is disallowed by robots.txt")
        .arg(crawlItem.url.toString());
    }
}

void RobotsHandler::setUserAgent(const QString &userAgent)
{
    m_userAgent = userAgent;
}
