#pragma once

#ifndef URLDATA_H
#define URLDATA_H

#include <QString>
#include <QMetaType>
#include <QTime>

struct UrlData {
    QString url;
    QTime time;
    quint16 statusCode;
    qint32 depth;
    qsizetype htmlSize;
};

Q_DECLARE_METATYPE(UrlData)

#endif // URLDATA_H
