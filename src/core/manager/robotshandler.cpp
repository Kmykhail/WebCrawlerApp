#include "robotshandler.h"

#include <QTextStream>
#include <QIODevice>
#include <QDebug>
#include <QRegularExpression>

constexpr bool isClieentError(quint16 statusCode) {
    return statusCode / 100 == 4;
}

constexpr bool isServerError(quint16 statusCode) {
    return statusCode / 100 == 5;
}

RobotsHandler::RobotsHandler(QObject *parent)
    : QObject{parent}
{
    connect(this, &RobotsHandler::evaluateWaitingItemsByHost, this, [this](const QString &host){
        if (auto it = m_waitingItemsByHost.find(host); it != m_waitingItemsByHost.end()) {
            QSet<CrawlItem> waitingItems = std::move(it.value());
            m_waitingItemsByHost.erase(it);

            const auto *rules = rulesForHost(host);
            if (rules) {
                waitingItems.removeIf([this, rules] (auto &item) {
                    return !evaluateItemWithRules(item, *rules);
                });
            }

            emit filtered(waitingItems);
        }
    });
}

const QList<RobotsHandler::RobotsRule> *RobotsHandler::rulesForHost(const QString &host) const {
    if (const auto hostIt = m_robotsRules.constFind(host);
        hostIt != m_robotsRules.constEnd()) {

        const auto &rulesByAgent = hostIt.value();
        const auto ruleIt = rulesByAgent.constFind(m_userAgent);

        if (ruleIt != rulesByAgent.constEnd()) {
            return &ruleIt.value();
        }

        if (const auto wildcarIt = rulesByAgent.constFind("*");
            wildcarIt != rulesByAgent.constEnd()) {
            return &wildcarIt.value();
        }
    }

    return nullptr;
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

        if (m_robotsRules.contains(host)) {
            if (const auto *rules = rulesForHost(host)) {
                hostItems.removeIf([this, rules](auto &item) {
                    return !evaluateItemWithRules(item, *rules);
                });
            }

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

    const auto &[item, content, statusCode, isSuccess] = respond;
    const auto &host = item.url.host();

    if (isClieentError(statusCode)) {
        qWarning() << "robots.txt unavailable:"
                   << item.url.toString()
                   << statusCode;
        m_robotsRules.insert(host, RobotsRulesByAgent{});
        emit evaluateWaitingItemsByHost(host);
        return;
    }

    if (isServerError(statusCode) || !isSuccess) {
        qWarning() << "robots.txt unreachable:"
                   << item.url.toString()
                   << statusCode;
        RobotsRulesByAgent denyAll;
        denyAll.insert(QStringLiteral("*"),
                       QList<RobotsRule>{{QStringLiteral("/"), false}});

        m_robotsRules.insert(host, std::move(denyAll));
        emit evaluateWaitingItemsByHost(host);
        return;
    }

    if (content.isEmpty()) {
        qWarning() << "robots.txt is empty:"
                   << item.url.toString();

        m_robotsRules.insert(host, RobotsRulesByAgent{});
        emit evaluateWaitingItemsByHost(host);
        return;
    }

    RobotsRulesByAgent rules;
    parse(content, rules);
    m_robotsRules.insert(host, std::move(rules));
    emit evaluateWaitingItemsByHost(host);
}

void RobotsHandler::clear()
{
    qDebug() << Q_FUNC_INFO;

    m_robotsRules.clear();
    m_waitingItemsByHost.clear();
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
        if (rule.path.isEmpty()) continue;

        QString pattern = rule.path;
        const bool endAnchored = pattern.endsWith('$');

        if (endAnchored) pattern.chop(1);

        pattern = QRegularExpression::escape(pattern);
        pattern.replace(QStringLiteral("\\*"), QStringLiteral(".*"));
        pattern.prepend('^');

        if (endAnchored) {
            pattern.append(QStringLiteral("\\z"));
        }

        const QRegularExpression regex(pattern);
        if (!regex.isValid() || !regex.match(path).hasMatch()) {
            continue;
        }

        const qsizetype matchLength = rule.path.toUtf8().size();

        if (matchLength > longestMatchLength ||
            (matchLength == longestMatchLength && rule.isAllowed && !isAllowed)) {
            longestMatchLength = matchLength;
            isAllowed = rule.isAllowed;
        }
    }

    if (!isAllowed) {
        qWarning() << QStringLiteral("URL skipped: %1 is disallowed by robots.txt").arg(crawlItem.url.toString());
    }

    return isAllowed;
}

void RobotsHandler::parse(const QByteArray &content, RobotsRulesByAgent &rules)
{
    QString text = QString::fromUtf8(content);
    QTextStream stream(&text, QIODevice::ReadOnly);

    QStringList currentUserAgents;
    bool rulesStarted = false;

    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        if (line.isEmpty()) continue;

        const qsizetype commentIndex = line.indexOf(u'#');
        if (commentIndex != -1) {
            line.truncate(commentIndex);
            line = line.trimmed();

            if (line.isEmpty()) continue;
        }

        const qsizetype colonIndex = line.indexOf(u':');
        if (colonIndex == -1) continue;

        const QString key = line.left(colonIndex).trimmed().toLower();
        const QString value = line.mid(colonIndex + 1).trimmed();

        if (key == QStringLiteral("user-agent")) {
            if (rulesStarted) {
                currentUserAgents.clear();
                rulesStarted = false;
            }

            if (!value.isEmpty()) {
                currentUserAgents.append(value.toLower());
            }

            continue;
        }

        if (key != QStringLiteral("allow") &&
            key != QStringLiteral("disallow")) {
            continue;
        }

        if (currentUserAgents.isEmpty() || value.isEmpty()) continue;

        const bool isAllowed = key == QStringLiteral("allow");

        for (const auto &userAgent : currentUserAgents) {
            rules[userAgent].append({
                value,
                isAllowed
            });
        }

        rulesStarted = true;
    }
}

void RobotsHandler::setUserAgent(const QString &userAgent)
{
    m_userAgent = userAgent.trimmed().toLower();
}
