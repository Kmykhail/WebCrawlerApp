#include "logmodel.h"

constexpr qsizetype LOG_LIMIT_SIZE = 1000;
constexpr qsizetype BATCH_SIZE = 100;

LogModel::LogModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_logData.reserve(LOG_LIMIT_SIZE + BATCH_SIZE);
}

LogModel &LogModel::instance()
{
    static LogModel obj;
    return obj;
}

int LogModel::rowCount(const QModelIndex &) const
{
    return m_logData.count();
}

QVariant LogModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_logData.count())
        return {};

    const auto &item = m_logData.at(index.row());
    switch (role) {
        case TimeRole: return item.m_time;
        case TypeRole: return item.type;
        case MessageRole: return item.message;
    }
    return {};
}

QHash<int, QByteArray> LogModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[TimeRole] = "time";
    roles[TypeRole] = "type";
    roles[MessageRole] = "message";

    return roles;
}

void LogModel::onAppendMessage(const LogData &logData)
{
    beginInsertRows(QModelIndex(), m_logData.size(), m_logData.size());
    m_logData.append(logData);
    endInsertRows();

    if (m_logData.size() >= LOG_LIMIT_SIZE + BATCH_SIZE) {
        beginRemoveRows(QModelIndex(), 0, BATCH_SIZE -1);
        m_logData.remove(0, BATCH_SIZE);
        endRemoveRows();
    }
}

void LogModel::clear()
{
    if (m_logData.isEmpty()) return;
    beginResetModel();
    m_logData.clear();
    endResetModel();
}
