#pragma once

#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractListModel>

#include "LogData.h"

// struct LogMessage {
//     static constexpr std::size_t MaxSize = 256;

//     std::array<char, MaxSize> text;
//     std::size_t size{0};

//     explicit LogMessage(const QString &msg) {
//         auto bytes = msg.toLocal8Bit();
//         size = std::min<std::size_t>(bytes.length(), MaxSize);
//         std::memcpy(text.data(), bytes.constData(), size);
//     }

//     std::string_view view() const noexcept {
//         return {text.data(), size};
//     }
// };

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
