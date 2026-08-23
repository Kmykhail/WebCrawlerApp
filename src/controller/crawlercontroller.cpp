#include "crawlercontroller.h"

CrawlerController::CrawlerController(QObject *parent)
    : QObject{parent}
    , m_manager{new CrawlerManager(this)}
    , m_model{new UrlModel(this)}
{
    connect(m_manager, &CrawlerManager::urlsDiscovered,
               m_model, &UrlModel::onUrlsDiscovered);
}

CrawlerManager *CrawlerController::manager() const
{
    return m_manager;
}

UrlModel *CrawlerController::model() const
{
    return m_model;
}
