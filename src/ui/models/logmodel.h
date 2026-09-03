#pragma once

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>
#include <QtQml/qqmlregistration.h>
#include <QQmlEngine>
#include <QJSEngine>
#include <QTime>

struct LogData {
    QTime m_time;
    QtMsgType type;
    QString message;
};

class LogModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
public:
    enum LogRoles{
        TimeRole = Qt::DisplayRole + 1,
        TypeRole,
        MessageRole
    };
    Q_ENUM(LogRoles)

    static LogModel &instance();

    static LogModel *create(QQmlEngine *, QJSEngine *)
    {
        QQmlEngine::setObjectOwnership(&instance(), QQmlEngine::CppOwnership);
        return &instance();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void clear();

public slots:
    void onAppendMessage(const LogData &logData);

private:
    explicit LogModel(QObject *parent = nullptr);
    QList<LogData> m_logData;
};

#endif // LOGMODEL_H
