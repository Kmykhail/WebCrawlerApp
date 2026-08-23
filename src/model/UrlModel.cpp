#include "UrlModel.h"
#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qobject.h>
#include <qstringview.h>
#include <qvariant.h>

UrlModel::UrlModel(QObject *parent) : QAbstractListModel(parent) {}

int UrlModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_urlData.count();
}

QVariant UrlModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_urlData.count())
        return {};

    switch (role) {
        case UrlRole: return m_urlData.at(index.row()).url;
        case TimeRole: return m_urlData.at(index.row()).time;
        case StatusRole: return m_urlData.at(index.row()).statusCode;
        case DepthRole: return m_urlData.at(index.row()).depth;
    }

    return {};
}

QHash<int, QByteArray> UrlModel::roleNames() const {
    QHash<int, QByteArray> roles;

    roles[TimeRole] = "TIME";
    roles[StatusRole] = "STATUS";
    roles[DepthRole] = "DEPTH";
    roles[UrlRole] = "URL";

    return roles;
}

void UrlModel::onUrlsDiscovered(const QList<UrlData> &batch)
{
    if (batch.isEmpty()) return;
    auto startRow = m_urlData.size();
    auto endRow = startRow + batch.size() - 1;

    beginInsertRows(QModelIndex(), startRow, endRow);
    m_urlData.append(batch);
    endInsertRows();
}
