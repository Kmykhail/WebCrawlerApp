#include <gtest/gtest.h>
#include <QCoreApplication>
#include "queuehandler.h"
#include "CrawlItem.h"

class QueueHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        queueHandler = new QueueHandler();
    }

    void TearDown() override {
        delete queueHandler;
    }

    QueueHandler *queueHandler;
};

TEST_F(QueueHandlerTest, InitialState) {
    EXPECT_TRUE(queueHandler->isQueueEmpty());
    EXPECT_EQ(queueHandler->queueSize(), 0);
    EXPECT_EQ(queueHandler->visitedSize(), 0);
    EXPECT_TRUE(queueHandler->isUnderLimit());
}

TEST_F(QueueHandlerTest, EnqueueAndDequeue) {
    QSet<CrawlItem> items = {
        {QUrl("http://example.com/1"), 0},
        {QUrl("http://example.com/2"), 0}
    };
    queueHandler->enqueue(items);
    EXPECT_FALSE(queueHandler->isQueueEmpty());
    EXPECT_EQ(queueHandler->queueSize(), 2);
    EXPECT_EQ(queueHandler->visitedSize(), 2);

    auto item1 = queueHandler->dequeue();
    EXPECT_TRUE(item1.has_value());
    EXPECT_EQ(queueHandler->queueSize(), 1);

    auto item2 = queueHandler->dequeue();
    EXPECT_TRUE(item2.has_value());
    EXPECT_EQ(queueHandler->queueSize(), 0);

    auto item3 = queueHandler->dequeue();
    EXPECT_FALSE(item3.has_value());
}

TEST_F(QueueHandlerTest, UrlLimit) {
    queueHandler->setUrlLimit(1);
    QSet<CrawlItem> items = {
        {QUrl("http://example.com/1"), 0},
        {QUrl("http://example.com/2"), 0}
    };
    queueHandler->enqueue(items);
    EXPECT_EQ(queueHandler->visitedSize(), 1);
    EXPECT_EQ(queueHandler->queueSize(), 1);
    EXPECT_FALSE(queueHandler->isUnderLimit());
}
