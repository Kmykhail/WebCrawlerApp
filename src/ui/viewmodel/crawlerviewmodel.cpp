#include "crawlerviewmodel.h"

CrawlerViewModel::CrawlerViewModel(QObject *parent)
    : QObject{parent}
    , m_manager{new CrawlerManager(this)}
    , m_model{new UrlModel(this)}
{
    m_state.controlState = static_cast<int>(m_manager->getControlState());

    connect(m_manager, &CrawlerManager::clearUrls,
            this, [this]() {
        m_model->onClear();

        m_state.reset();
        m_progress = 0.0;
        emit progressChanged();
        emit stateChanged();
    });

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
        updateProgress();

        m_model->onUrlsFetched(fetchBatch);
        emit stateChanged();
    });
}

CrawlerManager *CrawlerViewModel::manager() const
{
    return m_manager;
}

UrlModel *CrawlerViewModel::model() const
{
    return m_model;
}

CrawlerState CrawlerViewModel::state() const
{
    return m_state;
}

double CrawlerViewModel::progress() const
{
    return m_progress;
}

void CrawlerViewModel::updateProgress()
{
    auto limit = m_manager->getUrlLimit();
    if (!limit || limit >= std::numeric_limits<quint32>::max()) {
        return;
    }

    auto processed = m_state.fetched + m_state.failed;
    double progress = static_cast<double>(processed) / limit * 100.0;
    m_progress = qMin(100.0, progress);
    emit progressChanged();
}

LogModel *CrawlerViewModel::logModel() const
{
    return &LogModel::instance();
}
