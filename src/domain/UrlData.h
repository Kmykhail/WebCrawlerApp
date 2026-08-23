#pragma once

#ifndef URLDATA_H
#define URLDATA_H

#include <QString>
#include <QMetaType>
#include <QTime>

struct UrlData {
    QString url;
    QTime time;
    qint32 statusCode;
    qint32 depth;
};

Q_DECLARE_METATYPE(UrlData)

#endif // URLDATA_H
