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

TEST_F(RobotsHandlerTest, EvaluateUrl) {
    robotsHandler->clear();
    QSignalSpy spy(robotsHandler, &RobotsHandler::requiredRobotTxt);
    CrawlItem item{QUrl{"https://www.example.com"}, 0};

    robotsHandler->evaluateUrl(item);
    EXPECT_EQ(spy.count(), 1);

    auto args = spy.front();
    auto result = args.at(0).value<CrawlItem>();
    EXPECT_EQ(result.url, item.url);
}
