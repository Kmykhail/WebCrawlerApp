#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QUrl>
#include <QSet>
#include "worker.h"
#include "CrawlItem.h"

class WorkerTest : public ::testing::Test {
};

TEST_F(WorkerTest, EmptyHtml) {
    CrawlItem baseItem{QUrl("http://example.com"), 0};
    Worker worker(baseItem, QByteArray());

    bool finishedCalled = false;
    QSet<CrawlItem> extractedItems;
    QObject::connect(&worker, &Worker::finished, [&](const QSet<CrawlItem> &items) {
        finishedCalled = true;
        extractedItems = items;
    });

    worker.run();

    EXPECT_FALSE(finishedCalled);
}

TEST_F(WorkerTest, ExtractAndNormalizeLinks) {
    CrawlItem baseItem{QUrl("http://example.com/path/index.html"), 1};
    QByteArray html = R"(
        <html>
            <body>
                <a href="https://example.com/absolute">Absolute</a>
                <a href="/relative">Relative</a>
                <a href="page2.html">Sibling</a>
                <a href="invalid scheme">Invalid</a>
            </body>
        </html>
    )";

    Worker worker(baseItem, html);
    bool finishedCalled = false;
    QSet<CrawlItem> extractedItems;
    QObject::connect(&worker, &Worker::finished, [&](const QSet<CrawlItem> &items) {
        finishedCalled = true;
        extractedItems = items;
    });

    worker.run();

    EXPECT_TRUE(finishedCalled);
    EXPECT_GT(extractedItems.size(), 0);

    for (const auto &item : extractedItems) {
        EXPECT_EQ(item.depth, 2);
        EXPECT_TRUE(item.url.isValid());
        EXPECT_FALSE(item.url.host().isEmpty());
    }
}
