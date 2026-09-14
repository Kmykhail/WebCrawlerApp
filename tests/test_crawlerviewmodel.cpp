#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QSignalSpy>

#include "crawlerviewmodel.h"
#include "crawlermanager.h"

class CrawlerViewModelTest : public ::testing::Test {
protected:
    void SetUp() override {
        viewModel = new CrawlerViewModel();
    }

    void TearDown() override {
        delete viewModel;
    }

    CrawlerViewModel *viewModel;
};

TEST_F(CrawlerViewModelTest, InitialState) {
    EXPECT_NE(viewModel->manager(), nullptr);
    EXPECT_NE(viewModel->model(), nullptr);
    EXPECT_NE(viewModel->logModel(), nullptr);
    EXPECT_EQ(viewModel->progress(), 0.0);

    auto state = viewModel->state();
    EXPECT_FALSE(state.running);
    EXPECT_EQ(state.discovered, 0);
    EXPECT_EQ(state.queued, 0);
    EXPECT_EQ(state.fetched, 0);
    EXPECT_EQ(state.failed, 0);
}

TEST_F(CrawlerViewModelTest, ProgressChangedSignalAndCalculation) {
    QSignalSpy progressSpy(viewModel, &CrawlerViewModel::progressChanged);

    QList<UrlData> fetchBatch;
    CrawlItem item{QUrl("http://example.com/1"), 0};
    fetchBatch.append(UrlData{FetchResult{item, "<html></html>", 200, true}});

    emit viewModel->manager()->fetched(fetchBatch);

    EXPECT_GE(progressSpy.count(), 1);
    EXPECT_GT(viewModel->progress(), 0.0);
}

TEST_F(CrawlerViewModelTest, UrlsDiscoveredSignalHandling) {
    QSignalSpy stateSpy(viewModel, &CrawlerViewModel::stateChanged);
    QList<UrlData> batch;

    CrawlItem item{QUrl("http://example.com"), 0};

    FetchResult fetchResult{item, "<html></html>", 200, true};
    batch.append(UrlData{fetchResult});

    emit viewModel->manager()->urlsDiscovered(batch);

    EXPECT_GE(stateSpy.count(), 1);
    EXPECT_EQ(viewModel->state().discovered, 1);
}

TEST_F(CrawlerViewModelTest, ControlStateChangedSignalHandling) {
    QSignalSpy stateSpy(viewModel, &CrawlerViewModel::stateChanged);
    emit viewModel->manager()->controlStateChanged(CrawlerManager::ControlState::RUN);

    EXPECT_GE(stateSpy.count(), 1);
}

TEST_F(CrawlerViewModelTest, QueuedChangedSignalHandling) {
    QSignalSpy stateSpy(viewModel, &CrawlerViewModel::stateChanged);
    emit viewModel->manager()->queuedChanged(5);

    EXPECT_GE(stateSpy.count(), 1);
    EXPECT_EQ(viewModel->state().queued, 5);
}

TEST_F(CrawlerViewModelTest, FetchedSignalHandlingAndProgress) {
    viewModel->manager()->setUrlLimit("10");
    QSignalSpy stateSpy(viewModel, &CrawlerViewModel::stateChanged);
    QSignalSpy progressSpy(viewModel, &CrawlerViewModel::progressChanged);

    QList<UrlData> fetchBatch;
    CrawlItem item1{QUrl("http://example.com/1"), 0};
    CrawlItem item2{QUrl("http://example.com/2"), 0};

    fetchBatch.append(UrlData{FetchResult{item1, "<html></html>", 200, true}});
    fetchBatch.append(UrlData{FetchResult{item2, "<html></html>", 404, false}});
    emit viewModel->manager()->fetched(fetchBatch);

    EXPECT_GE(stateSpy.count(), 1);
    EXPECT_GE(progressSpy.count(), 1);
    EXPECT_EQ(viewModel->state().fetched, 1);
    EXPECT_EQ(viewModel->state().failed, 1);
    EXPECT_GT(viewModel->progress(), 0.0);
}
