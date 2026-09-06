#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QThread>
#include "urlfetcher.h"
#include "CrawlItem.h"

class UrlFetcherTest : public ::testing::Test {
protected:
    void SetUp() override {
        urlFetcher = new UrlFetcher();
    }

    void TearDown() override {
        delete urlFetcher;
    }

    UrlFetcher *urlFetcher;
};

TEST_F(UrlFetcherTest, InitialState) {
    EXPECT_EQ(urlFetcher->activeDownloads(), 0);
}

TEST_F(UrlFetcherTest, CustomUserAgent) {
    urlFetcher->setCustomUserAgent("TestAgent/1.0");
    SUCCEED();
}

TEST_F(UrlFetcherTest, AbortNetworkReplies) {
    CrawlItem item{QUrl("http://127.0.0.1:12345/slow"), 0};
    urlFetcher->executeNetworkRequest(item);
    EXPECT_EQ(urlFetcher->activeDownloads(), 1);

    urlFetcher->abortNetworkReplies();
    EXPECT_EQ(urlFetcher->activeDownloads(), 0);
}

TEST_F(UrlFetcherTest, TimeoutHandling) {
    QSignalSpy spy(urlFetcher, &UrlFetcher::fetched);
    
    CrawlItem item{QUrl("http://10.255.255.1:12345/timeut"), 0};
    urlFetcher->executeNetworkRequest(item);

    bool signalaed = spy.wait(6000);
    EXPECT_TRUE(signalaed);

    if (!spy.isEmpty()) {
        auto args = spy.takeFirst();
        auto result = args.at(0).value<FetchResult>();
        EXPECT_FALSE(result.success);
        EXPECT_EQ(result.statusCode, 499);
    }
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
