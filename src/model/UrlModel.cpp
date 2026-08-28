#include "UrlModel.h"
#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qobject.h>
#include <qstringview.h>
#include <qvariant.h>

UrlModel::UrlModel(QObject *parent) : QAbstractTableModel(parent) {}

int UrlModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_urlData.count();
}

int UrlModel::columnCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return 5;
}

QVariant UrlModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_urlData.count())
        return {};

    const UrlData &item = m_urlData.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return item.url;
            case 1: return item.time;
            case 2: return item.statusCode;
            case 3: return item.depth;
            default: return {};
        }
    }

    switch (role) {
        case UrlRole: return item.url;
        case TimeRole: return item.time;
        case StatusRole: return item.statusCode;
        case DepthRole: return item.depth;
    }

    return {};
}

QHash<int, QByteArray> UrlModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[Qt::DisplayRole] = "display";
    roles[UrlRole] = "url";
    roles[TimeRole] = "time";
    roles[StatusRole] = "status";
    roles[DepthRole] = "depth";

    return roles;
}

void UrlModel::onUrlsDiscovered(const QList<UrlData> &batch)
{
    if (batch.isEmpty()) return;
    auto startRow = m_urlData.size();
    auto endRow = startRow + batch.size() - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);

    int index = startRow;
    for (const auto &urlData: batch) {
        m_urlHashIndex.insert(qHash(urlData.url), index++);
    }
    m_urlData.append(batch);

    endInsertRows();
}

void UrlModel::onUpdateStatuses(const QHash<QString, quint16> &statusCodes)
{
    for (const auto &[key, value]: statusCodes.asKeyValueRange()) {
        if (auto rowIndex = findRowByKey(key); rowIndex != -1) {
            m_urlData[rowIndex].statusCode = value;
            QModelIndex targetIndex = index(rowIndex, 2);
            emit dataChanged(targetIndex, targetIndex, {Qt::DisplayRole, StatusRole});
        }
    }
}

qint32 UrlModel::findRowByKey(const QString &key) const
{
    return m_urlHashIndex.value(qHash(key), -1);
}
