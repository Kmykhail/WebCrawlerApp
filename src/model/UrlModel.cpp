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
    return ColumnCount;
}

QVariant UrlModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_urlData.count())
        return {};

    const UrlData &item = m_urlData.at(index.row());

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case NumberColumn: return index.row() + 1;
            case UrlColumn: return item.url;
            case TimeColumn: return item.time;
            case StatusColumn: return item.statusCode;
            case DepthColumn: return item.depth;
            case SizeColumn: return item.htmlSize;
            default: return {};
        }
    }

    switch (role) {
        case UrlRole: return item.url;
        case TimeRole: return item.time;
        case StatusRole: return item.statusCode;
        case DepthRole: return item.depth;
        case SizeRole: return item.htmlSize;
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
    roles[SizeRole] = "htmlSize";

    return roles;
}

void UrlModel::onUrlsDiscovered(const QList<UrlData> &rawBatch)
{
    if (rawBatch.isEmpty()) return;
    auto startRow = m_urlData.size();
    auto endRow = startRow + rawBatch.size() - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);

    int index = startRow;
    for (const auto &urlData: rawBatch) {
        m_urlHashIndex.insert(qHash(urlData.url), index++);
    }
    m_urlData.append(rawBatch);

    endInsertRows();
}

void UrlModel::onUrlsFetched(const QList<UrlData> &fetchBatch)
{
    for (const auto &[url, time, statusCode, depth, size]: fetchBatch) {
        if (statusCode == 0 && size == 0) {
            qWarning() << QStringLiteral("fetched url: %1 has invalid status: 0 and size: 0").arg(url);
            continue;
        }

        if (auto rowIndex = findRowByKey(url); rowIndex != -1) {
            m_urlData[rowIndex].statusCode = statusCode;
            m_urlData[rowIndex].htmlSize = size;
            QModelIndex targetIndex = index(rowIndex, StatusColumn);
            emit dataChanged(targetIndex, index(rowIndex, SizeColumn), {Qt::DisplayRole, StatusRole, SizeRole});
        }
    }
}

qint32 UrlModel::findRowByKey(const QString &key) const
{
    return m_urlHashIndex.value(qHash(key), -1);
}
