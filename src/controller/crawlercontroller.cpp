#include "crawlercontroller.h"

CrawlerController::CrawlerController(QObject *parent)
    : QObject{parent}
    , m_manager{new CrawlerManager(this)}
    , m_model{new UrlModel(this)}
{
    connect(m_manager, &CrawlerManager::urlsDiscovered,
            m_model, &UrlModel::onUrlsDiscovered);
    connect(m_manager, &CrawlerManager::urlsDiscovered,
            this, [this](const QList<UrlData> &batch) {
        m_state.discovered += batch.size();
        emit stateChanged();
    });
    connect(m_manager, &CrawlerManager::controlStateChanged,
            this, [this](){
        m_state.controlState = m_manager->getControlState();
        m_state.running = m_state.controlState == CrawlerManager::RUN ||
                          m_state.controlState == CrawlerManager::RESUME;
        emit stateChanged();
    });

    connect(m_manager, &CrawlerManager::urlsFetched,
            m_model, &UrlModel::onUrlsFetched);
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
