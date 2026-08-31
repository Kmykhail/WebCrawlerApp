#pragma once

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>

#include "LogData.h"

class LogModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum LogRoles{
        TimeRole = Qt::DisplayRole + 1,
        TypeRole,
        MessageRole
    };
    Q_ENUM(LogRoles)

    static LogModel &instance();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void clear();

public slots:
    void onAppendMessage(const LogData &logData);

private:
    explicit LogModel(QObject *parent = nullptr);

private:
    QList<LogData> m_logData;
};

#endif // LOGMODEL_H
