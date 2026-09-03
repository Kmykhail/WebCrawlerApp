#include "crawlercontroller.h"

CrawlerController::CrawlerController(QObject *parent)
    : QObject{parent}
    , m_manager{new CrawlerManager(this)}
    , m_model{new UrlModel(this)}
{
    m_state.controlState = static_cast<int>(m_manager->getControlState());

    connect(m_manager, &CrawlerManager::urlsDiscovered,
            this, [this](const QList<UrlData> &batch) {
        m_state.discovered += batch.size();
        m_model->onUrlsDiscovered(batch);
        emit stateChanged();
    });
    connect(m_manager, &CrawlerManager::controlStateChanged,
            this, [this](){
        m_state.controlState = static_cast<int>(m_manager->getControlState());
        m_state.running = m_manager->getControlState() == CrawlerManager::RUN ||
                          m_manager->getControlState() == CrawlerManager::RESUME;
        emit stateChanged();
    });

    connect(m_manager, &CrawlerManager::queuedChanged,
            this, [this](qsizetype queued) {
        m_state.queued = queued;
        emit stateChanged();
    });

    connect(m_manager, &CrawlerManager::fetched,
            this, [this](const QList<UrlData> &fetchBatch){
        auto failedCount = std::ranges::count_if(fetchBatch, [](const UrlData &data) {
            return data.statusCode != 200;
        });
        auto successCount = fetchBatch.size() - failedCount;

        m_state.fetched += successCount;
        m_state.failed += failedCount;

        m_model->onUrlsFetched(fetchBatch);
        emit stateChanged();
    });

}

CrawlerManager *CrawlerController::manager() const
{
    return m_manager;
}

UrlModel *CrawlerController::model() const
{
    return m_model;
}

CrawlerState CrawlerController::state() const
{
    return m_state;
}

LogModel *CrawlerController::logModel() const
{
    return &LogModel::instance();
}
