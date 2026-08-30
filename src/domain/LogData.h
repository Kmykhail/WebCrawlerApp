#pragma once

#ifndef LOGDATA_H
#define LOGDATA_H

#include <QMetaType>
#include <QTime>

struct LogData {
    QTime m_time;
    QtMsgType type;
    QString message;
};

Q_DECLARE_METATYPE(LogData)

#endif // LOGDATA_H
