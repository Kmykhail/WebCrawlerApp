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

    RobotsHandler *robotsHandler{nullptr};
};

TEST_F(RobotsHandlerTest, EvaluateUrlTriggersFetch) {
    QSignalSpy requiredParsingSpy(robotsHandler, &RobotsHandler::requiredRobotTxt);
    QSignalSpy filteredItemsSpy(robotsHandler, &RobotsHandler::filtered);

    CrawlItem item{QUrl{"https://www.example.com"}, 0};

    robotsHandler->evaluateUrl(item);
    EXPECT_EQ(requiredParsingSpy.count(), 1);
    EXPECT_EQ(filteredItemsSpy.count(), 0);

    {
        auto args = requiredParsingSpy.front();
        auto result = args.at(0).value<CrawlItem>();
        EXPECT_EQ(result.url, item.url);
    }
}

TEST_F(RobotsHandlerTest, ParseRobotsTxtSuccessReleasesWaitingItems) {
    QSignalSpy requiredParsingSpy(robotsHandler, &RobotsHandler::requiredRobotTxt);
    QSignalSpy filteredItemsSpy(robotsHandler, &RobotsHandler::filtered);

    CrawlItem item{QUrl{"https://www.example.com"}, 0};

    robotsHandler->evaluateUrl(item);
    EXPECT_EQ(requiredParsingSpy.count(), 1);

    FetchResult fetchResult{
        CrawlItem{QUrl{"https://www.example.com/allowed-page"}, 0},
        "User-agent: *\nDisallow: /secret",
        200,
        true
    };
    robotsHandler->parseRobotsTxt(fetchResult);

    EXPECT_EQ(filteredItemsSpy.count(), 1);
    auto allowedItems = filteredItemsSpy.front().at(0).value<QSet<CrawlItem>>();
    EXPECT_TRUE(allowedItems.contains(item));
}

TEST_F(RobotsHandlerTest, ParseRobotsTxtFailureFallsBackToAllow) {
    QSignalSpy filteredItemsSpy(robotsHandler, &RobotsHandler::filtered);
    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};
    robotsHandler->evaluateUrl(item);

    FetchResult fetchResult{
        CrawlItem{QUrl{"https://www.example.com/robots.txt"}, 0},
        "",
        404,
        false
    };
    robotsHandler->parseRobotsTxt(fetchResult);
    EXPECT_EQ(filteredItemsSpy.count(), 1);
    auto allowedItems = filteredItemsSpy.front().at(0).value<QSet<CrawlItem>>();
    EXPECT_TRUE(allowedItems.contains(item));
}

TEST_F(RobotsHandlerTest, RulesMatchingDisallowAndAllow) {
    QSignalSpy filteredItemsSpy(robotsHandler, &RobotsHandler::filtered);

    FetchResult fetchResult{
        CrawlItem{QUrl{"https://www.example.com/robots.txt"}, 0},
        "User-agent: *\nDisallow: /private\nAllow: /private/public-folder",
        200,
        true
    };
    robotsHandler->parseRobotsTxt(fetchResult);
    CrawlItem disallowItem{QUrl{"https://www.example.com/private/secret.html"}, 0};
    CrawlItem allowItem{QUrl{"https://www.example.com/private/public-folder/file.html"}, 0};

    robotsHandler->evaluateUrls({disallowItem, allowItem});
    EXPECT_EQ(filteredItemsSpy.count(), 1);

    QSet<CrawlItem> allEmittedItems;
    for (const auto &signalArg: filteredItemsSpy) {
        allEmittedItems.unite(signalArg.at(0).value<QSet<CrawlItem>>());
    }

    EXPECT_TRUE(allEmittedItems.contains(allowItem));
    EXPECT_FALSE(allEmittedItems.contains(disallowItem));
}

TEST_F(RobotsHandlerTest, UserAgentSpecificRules) {
    QSignalSpy filteredItemsSpy(robotsHandler, &RobotsHandler::filtered);
    robotsHandler->setUserAgent("MyCustomBot");
    FetchResult fetchResult{
        CrawlItem{QUrl{"https://www.example.com/robots.txt"}, 0},
        "User-agent: *\nDisallow: /\nUser-agent: MyCustomBot\nAllow: /",
        200,
        true
    };
    robotsHandler->parseRobotsTxt(fetchResult);
    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};
    robotsHandler->evaluateUrl(item);

    EXPECT_EQ(filteredItemsSpy.count(), 1);
}

TEST_F(RobotsHandlerTest, ClearResetsState) {
    QSignalSpy requiredParsingSpy(robotsHandler, &RobotsHandler::requiredRobotTxt);
    CrawlItem item{QUrl{"https://www.example.com/page"}, 0};
    robotsHandler->evaluateUrl(item);
    EXPECT_EQ(requiredParsingSpy.count(), 1);

    robotsHandler->clear();
    requiredParsingSpy.clear();
    EXPECT_EQ(requiredParsingSpy.count(), 0);
    robotsHandler->evaluateUrl(item);
    EXPECT_EQ(requiredParsingSpy.count(), 1);
}
