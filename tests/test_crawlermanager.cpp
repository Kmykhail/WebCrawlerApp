#include <gtest/gtest.h>
#include <QCoreApplication>

#include "crawlermanager.h"

class CrawlerManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        crawlerManager = new CrawlerManager();
    }

    void TearDown() override {
        delete crawlerManager;
    }

    CrawlerManager *crawlerManager;
};

TEST_F(CrawlerManagerTest, InitialState) {
    EXPECT_EQ(crawlerManager->getControlState(), CrawlerManager::ControlState::IDLE);
}

TEST_F(CrawlerManagerTest, SetUrlDepth) {
    crawlerManager->setUrlDepth(3);
    EXPECT_EQ(crawlerManager->getControlState(), CrawlerManager::ControlState::IDLE);
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
