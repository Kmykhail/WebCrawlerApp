#pragma once

#ifndef CRAWLERCONTROLLER_H
#define CRAWLERCONTROLLER_H

#include <QObject>

#include "UrlModel.h"
#include "crawlermanager.h"

class CrawlerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CrawlerManager* manager READ manager CONSTANT)
    Q_PROPERTY(UrlModel* model READ model CONSTANT)

public:
    explicit CrawlerController(QObject *parent = nullptr);

    CrawlerManager *manager() const;
    UrlModel *model() const;

private:
    CrawlerManager *m_manager{nullptr};
    UrlModel *m_model{nullptr};
};

#endif // CRAWLERCONTROLLER_H
