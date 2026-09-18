#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSignalSpy>
#include <QThread>
#include "urlfetcher.h"
#include "CrawlItem.h"
#include "MockServer/mockhttpserver.h"

namespace {
constexpr quint32 timeout_ms = 6000;
}

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
    MockHttpServer server;
    ASSERT_TRUE(server.listen());
    QSignalSpy spy(urlFetcher, &UrlFetcher::fetched);

    server.addRoute("/slow", 200, "timeout", timeout_ms);
    QString targetUrl = QString("http://127.0.0.1:%1/slow").arg(server.port());
    CrawlItem item{QUrl{targetUrl}, 0};
    urlFetcher->executeNetworkRequest(item);

    bool signalaed = spy.wait(timeout_ms + 500);
    EXPECT_TRUE(signalaed);

    auto args = spy.takeFirst();
    auto result = args.at(0).value<FetchResult>();
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.statusCode, 499);
}

TEST_F(UrlFetcherTest, FetchFromMockServer) {
    MockHttpServer server;
    ASSERT_TRUE(server.listen());

    server.addRoute("/test", 200, "<html><body>Hello Mock Server</body></html>");
    QSignalSpy spy(urlFetcher, &UrlFetcher::fetched);

    QString targetUrl = QString("http://127.0.0.1:%1/test").arg(server.port());
    CrawlItem item{QUrl{targetUrl}, 0};

    urlFetcher->executeNetworkRequest(item);

    bool signaled = spy.wait(1000);
    EXPECT_TRUE(signaled);

    auto args = spy.takeFirst();
    auto result = args.at(0).value<FetchResult>();
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.html, "<html><body>Hello Mock Server</body></html>");
    EXPECT_EQ(result.statusCode, 200);
}

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
