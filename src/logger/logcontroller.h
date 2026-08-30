#pragma once

#ifndef LogController_H
#define LogController_H

#include <QObject>
// #include "logmodel.h"

class LogController : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(LogController)
public:
    static LogController &instance();
    void logger(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    LogController(QObject *parent = nullptr);
    ~LogController() = default;
};

#endif // LOGHANDLER_H
