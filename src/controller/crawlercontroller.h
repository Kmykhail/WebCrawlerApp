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
    Q_PROPERTY(qint32 discovered READ discovered NOTIFY discoveredChanged FINAL)

public:
    explicit CrawlerController(QObject *parent = nullptr);

    CrawlerManager *manager() const;
    UrlModel *model() const;
    qint32 discovered() const;

signals:
    void discoveredChanged();

private:
    CrawlerManager *m_manager{nullptr};
    UrlModel *m_model{nullptr};
    qint32 m_discovered{0};
};

#endif // CRAWLERCONTROLLER_H
