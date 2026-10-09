#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSignalSpy>

#include "robotshandler.h"
#include "CrawlItem.h"

class RobotsHandlerTest: public ::testing::Test {
protected:
    void SetUp() override {
        robotsHandler = new RobotsHandler();
    }

    void TearDown() override {
        delete robotsHandler;
    }

    FetchResult makeRobotsResponse(
        const QString &host,
        QByteArray content = {},
        quint16 statusCode = 200,
        bool isSuccess = true) const
    {
        return {
            CrawlItem{
                QUrl{QStringLiteral("https://%1/robots.txt").arg(host)},
                0
            },
            std::move(content),
            statusCode,
            isSuccess
        };
    }

    void loadRobots(const QString &host, const QByteArray &content) {
        robotsHandler->parseRobotsTxt(
            makeRobotsResponse(host, content)
        );
    }

    QSet<CrawlItem> collectFilteredItems(const QSignalSpy &spy) const
    {
        QSet<CrawlItem> result;

        for (const auto &arguments : spy) {
            result.unite(arguments.at(0).value<QSet<CrawlItem>>());
        }

        return result;
    }

    RobotsHandler *robotsHandler{nullptr};
};

TEST_F(RobotsHandlerTest, EvaluateUrlTriggersFetch) {
    QSignalSpy requiredSpy(robotsHandler, &RobotsHandler::requiredRobotTxt);
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    CrawlItem item{QUrl{"https://www.example.com"}, 0};

    robotsHandler->evaluateUrl(item);
    ASSERT_EQ(requiredSpy.count(), 1);
    EXPECT_EQ(filteredSpy.count(), 0);

    auto args = requiredSpy.front();
    auto result = args.at(0).value<CrawlItem>();
    EXPECT_EQ(result.url, item.url);
}

TEST_F(RobotsHandlerTest, MultipleUrlsForSameHostTriggerOnlyOneFetch) {
    QSignalSpy requiredSpy(robotsHandler, &RobotsHandler::requiredRobotTxt);
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    CrawlItem allowed{QUrl{"https://www.example.com/public"}, 0};
    CrawlItem blocked{QUrl{"https://www.example.com/private"}, 0};

    robotsHandler->evaluateUrls({allowed, blocked});

    ASSERT_EQ(requiredSpy.count(), 1);

    loadRobots(
        "www.example.com",
        "User-agent: *\n"
        "Disallow: /private"
    );

    ASSERT_EQ(filteredSpy.count(), 1);

    const auto result = collectFilteredItems(filteredSpy);

    EXPECT_TRUE(result.contains(allowed));
    EXPECT_FALSE(result.contains(blocked));
}

TEST_F(RobotsHandlerTest, ParseRobotsTxtReleasesWaitingItems)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    CrawlItem item{QUrl{"https://www.example.com/allowed"}, 0};

    robotsHandler->evaluateUrl(item);

    robotsHandler->parseRobotsTxt(makeRobotsResponse(
        "www.example.com",
        "User-agent: *\nDisallow: /secret"));

    EXPECT_TRUE(collectFilteredItems(filteredSpy).contains(item));
}

TEST_F(RobotsHandlerTest, Http4xxFallsBackToAllow)
{
    const QList<int> statusCodes{400, 403, 404, 429};

    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};

    for (const int statusCode : statusCodes) {
        robotsHandler->clear();

        QSignalSpy filteredSpy(
            robotsHandler, &RobotsHandler::filtered);

        robotsHandler->evaluateUrl(item);

        robotsHandler->parseRobotsTxt(makeRobotsResponse(
            "www.example.com",
            {},
            statusCode,
            false));

        EXPECT_TRUE(collectFilteredItems(filteredSpy).contains(item))
            << "Unexpected result for HTTP " << statusCode;
    }
}

TEST_F(RobotsHandlerTest, Http5xxDisallowsAllUrls)
{
    const QList<int> statusCodes{500, 502, 503, 504};

    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};

    for (const int statusCode : statusCodes) {
        robotsHandler->clear();

        QSignalSpy filteredSpy(
            robotsHandler, &RobotsHandler::filtered);

        robotsHandler->evaluateUrl(item);

        robotsHandler->parseRobotsTxt(makeRobotsResponse(
            "www.example.com",
            {},
            statusCode,
            false));

        EXPECT_FALSE(collectFilteredItems(filteredSpy).contains(item))
            << "Unexpected result for HTTP " << statusCode;
    }
}

TEST_F(RobotsHandlerTest, NetworkErrorDisallowsAllUrls)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};

    robotsHandler->evaluateUrl(item);

    robotsHandler->parseRobotsTxt(makeRobotsResponse(
        "www.example.com",
        {},
        0,
        false));

    EXPECT_FALSE(collectFilteredItems(filteredSpy).contains(item));
}

TEST_F(RobotsHandlerTest, EmptySuccessfulResponseAllowsAllUrls)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};

    robotsHandler->evaluateUrl(item);

    robotsHandler->parseRobotsTxt(makeRobotsResponse(
        "www.example.com",
        {},
        200,
        true));

    EXPECT_TRUE(collectFilteredItems(filteredSpy).contains(item));
}

TEST_F(RobotsHandlerTest, MoreSpecificAllowOverridesDisallow)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    loadRobots(
        "www.example.com",
        "User-agent: *\n"
        "Disallow: /private\n"
        "Allow: /private/public-folder");

    CrawlItem blocked{QUrl{"https://www.example.com/private/secret.html"}, 0};
    CrawlItem allowed{QUrl{"https://www.example.com/private/public-folder/file.html"}, 0};

    robotsHandler->evaluateUrls({blocked, allowed});

    const auto result = collectFilteredItems(filteredSpy);

    EXPECT_TRUE(result.contains(allowed));
    EXPECT_FALSE(result.contains(blocked));
}

TEST_F(RobotsHandlerTest, AllowWinsWhenSpecificityIsEqual)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    // Disallow appears first.
    loadRobots(
        "first.example.com",
        "User-agent: *\n"
        "Disallow: /same\n"
        "Allow: /same");

    // Allow appears first.
    loadRobots(
        "second.example.com",
        "User-agent: *\n"
        "Allow: /same\n"
        "Disallow: /same");

    CrawlItem first{QUrl{"https://first.example.com/same/page"}, 0};
    CrawlItem second{QUrl{"https://second.example.com/same/page"}, 0};

    robotsHandler->evaluateUrls({first, second});

    const auto result = collectFilteredItems(filteredSpy);

    EXPECT_TRUE(result.contains(first));
    EXPECT_TRUE(result.contains(second));
}

TEST_F(RobotsHandlerTest, WildcardAndEndAnchorAreMatched)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    loadRobots(
        "www.example.com",
        "User-agent: *\n"
        "Disallow: /private/*.pdf$");

    CrawlItem blocked{QUrl{"https://www.example.com/private/report.pdf"}, 0};
    CrawlItem allowedDifferentExtension{QUrl{"https://www.example.com/private/report.txt"}, 0};
    CrawlItem allowedSuffix{QUrl{"https://www.example.com/private/report.pdf/extra"}, 0};

    robotsHandler->evaluateUrls({
        blocked,
        allowedDifferentExtension,
        allowedSuffix
    });

    const auto result = collectFilteredItems(filteredSpy);

    EXPECT_FALSE(result.contains(blocked));
    EXPECT_TRUE(result.contains(allowedDifferentExtension));
    EXPECT_TRUE(result.contains(allowedSuffix));
}

TEST_F(RobotsHandlerTest, EmptyDisallowDoesNotBlockAnything)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    loadRobots(
        "www.example.com",
        "User-agent: *\n"
        "Disallow:\n");

    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};

    robotsHandler->evaluateUrl(item);

    EXPECT_TRUE(collectFilteredItems(filteredSpy).contains(item));
}


TEST_F(RobotsHandlerTest, RulesAreIsolatedPerHost)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    loadRobots(
        "first.example.com",
        "User-agent: *\n"
        "Disallow: /private");

    loadRobots(
        "second.example.com",
        "User-agent: *\n"
        "Disallow: /admin");

    CrawlItem first{QUrl{"https://first.example.com/private/page"}, 0};
    CrawlItem second{QUrl{"https://second.example.com/private/page"}, 0};

    robotsHandler->evaluateUrls({first, second});

    const auto result = collectFilteredItems(filteredSpy);

    EXPECT_FALSE(result.contains(first));
    EXPECT_TRUE(result.contains(second));
}

TEST_F(RobotsHandlerTest, SpecificUserAgentOverridesWildcardGroup)
{
    QSignalSpy filteredSpy(
        robotsHandler, &RobotsHandler::filtered);

    robotsHandler->setUserAgent("MyCustomBot");

    loadRobots(
        "www.example.com",
        "User-agent: *\n"
        "Disallow: /\n"
        "User-agent: MyCustomBot\n"
        "Allow: /");

    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};

    robotsHandler->evaluateUrl(item);

    EXPECT_TRUE(collectFilteredItems(filteredSpy).contains(item));
}

TEST_F(RobotsHandlerTest, RulesDoNotLeakIntoNextUserAgentGroup)
{
    QSignalSpy filteredSpy(robotsHandler, &RobotsHandler::filtered);

    robotsHandler->setUserAgent("BotTwo");

    loadRobots(
        "www.example.com",
        "User-agent: BotOne\n"
        "Disallow: /bot-one\n"
        "\n"
        "User-agent: BotTwo\n"
        "Disallow: /bot-two");

    CrawlItem botOnePath{
                         QUrl{"https://www.example.com/bot-one/page"}, 0};
    CrawlItem botTwoPath{
                         QUrl{"https://www.example.com/bot-two/page"}, 0};

    robotsHandler->evaluateUrls({botOnePath, botTwoPath});

    const auto result = collectFilteredItems(filteredSpy);

    EXPECT_TRUE(result.contains(botOnePath));
    EXPECT_FALSE(result.contains(botTwoPath));
}

TEST_F(RobotsHandlerTest, ClearResetsState) {
    QSignalSpy requiredSpy(robotsHandler, &RobotsHandler::requiredRobotTxt);
    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};
    robotsHandler->evaluateUrl(item);
    EXPECT_EQ(requiredSpy.count(), 1);

    robotsHandler->clear();
    requiredSpy.clear();
    EXPECT_EQ(requiredSpy.count(), 0);
    robotsHandler->evaluateUrl(item);
    EXPECT_EQ(requiredSpy.count(), 1);
}
