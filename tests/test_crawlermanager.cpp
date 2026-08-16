#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>

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
    EXPECT_FALSE(crawlerManager->isRunning());
}

TEST_F(CrawlerManagerTest, StartAndPause) {
    crawlerManager->start("http://example.com");
    EXPECT_TRUE(crawlerManager->isRunning());

    crawlerManager->pause();
    EXPECT_FALSE(crawlerManager->isRunning());   
}

TEST_F(CrawlerManagerTest, CheckUrlQueue) {
    const CrawlItem item{
        QUrl("http://example.com"),
        0
    };

    crawlerManager->m_urlQueue.enqueue(item);
    crawlerManager->m_controlState = CrawlerManager::PAUSE;

    const CrawlItem dequeuedItem = crawlerManager->m_urlQueue.dequeue();

    ASSERT_TRUE(crawlerManager->m_urlQueue.isEmpty());

    crawlerManager->loadHtml(dequeuedItem);

    ASSERT_EQ(crawlerManager->m_urlQueue.size(), 1);

    const CrawlItem &result = crawlerManager->m_urlQueue.front();

    EXPECT_EQ(result.url, item.url);
    EXPECT_EQ(result.depth, item.depth);
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
