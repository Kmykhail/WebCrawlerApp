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
        m_discovered += batch.size();
        emit discoveredChanged();
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

qint32 CrawlerController::discovered() const
{
    return m_discovered;
}
