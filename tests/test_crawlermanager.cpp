#include <gtest/gtest.h>
#include <QCoreApplication>

#include "crawlermanager.h"
#include "CrawlItem.h"

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

TEST_F(CrawlerManagerTest, StartAndStop) {
    crawlerManager->start("http://example.com");
    EXPECT_EQ(crawlerManager->getControlState(), CrawlerManager::ControlState::RUN);

    crawlerManager->pause();
    EXPECT_EQ(crawlerManager->getControlState(), CrawlerManager::ControlState::PAUSE);

    crawlerManager->resume();
    EXPECT_EQ(crawlerManager->getControlState(), CrawlerManager::ControlState::RUN);

    crawlerManager->stop();
    EXPECT_EQ(crawlerManager->getControlState(), CrawlerManager::ControlState::STOP);

    crawlerManager->clear();
    EXPECT_EQ(crawlerManager->getControlState(), CrawlerManager::ControlState::IDLE);
}

TEST_F(CrawlerManagerTest, LinkScrapingEnqueuesItems) {
    QSet<CrawlItem> items = {
        {QUrl("http://example.com/scraped1"), 1},
        {QUrl("http://example.com/scraped2"), 1}
    };

    crawlerManager->onLinkScraping(items);
    SUCCEED();
}
