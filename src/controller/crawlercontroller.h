#pragma once

#ifndef CRAWLERCONTROLLER_H
#define CRAWLERCONTROLLER_H

#include <QObject>

#include "UrlModel.h"
#include "crawlermanager.h"
#include "CrawlerState.h"

class CrawlerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CrawlerManager* manager READ manager CONSTANT)
    Q_PROPERTY(UrlModel* model READ model CONSTANT)
    Q_PROPERTY(CrawlerState state READ state NOTIFY stateChanged)

public:
    explicit CrawlerController(QObject *parent = nullptr);

    CrawlerManager *manager() const;
    UrlModel *model() const;
    CrawlerState state() const;

signals:
    void stateChanged();

private:
    CrawlerManager *m_manager{nullptr};
    UrlModel *m_model{nullptr};
    CrawlerState m_state;
};

#endif // CRAWLERCONTROLLER_H
