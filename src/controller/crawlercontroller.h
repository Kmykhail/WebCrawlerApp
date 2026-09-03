#pragma once

#ifndef CRAWLERCONTROLLER_H
#define CRAWLERCONTROLLER_H

#include <QObject>
#include <QtQml/qqmlregistration.h>

#include "models/urlmodel.h"
#include "models/logmodel.h"
#include "crawlermanager.h"
#include "CrawlerState.h"

class CrawlerController : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(CrawlerManager* manager READ manager CONSTANT)
    Q_PROPERTY(UrlModel* model READ model CONSTANT)
    Q_PROPERTY(LogModel* logModel READ logModel CONSTANT)
    Q_PROPERTY(CrawlerState state READ state NOTIFY stateChanged)

public:
    enum class ControlState {
        RUN = static_cast<int>(CrawlerManager::ControlState::RUN),
        PAUSE = static_cast<int>(CrawlerManager::ControlState::PAUSE),
        RESUME = static_cast<int>(CrawlerManager::ControlState::RESUME),
        STOP = static_cast<int>(CrawlerManager::ControlState::STOP),
        IDLE = static_cast<int>(CrawlerManager::ControlState::IDLE)
    };
    Q_ENUM(ControlState)

    explicit CrawlerController(QObject *parent = nullptr);

    CrawlerManager *manager() const;
    UrlModel *model() const;
    LogModel *logModel() const;
    CrawlerState state() const;

signals:
    void stateChanged();

private:
    CrawlerManager *m_manager{nullptr};
    UrlModel *m_model{nullptr};
    CrawlerState m_state;
};

#endif // CRAWLERCONTROLLER_H
