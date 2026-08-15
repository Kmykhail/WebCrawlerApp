#include "UrlModel.h"
#include <qabstractitemmodel.h>
#include <qhash.h>
#include <qobject.h>
#include <qstringview.h>
#include <qvariant.h>

UrlModel::UrlModel(QObject *parent) : QAbstractListModel(parent) {}

int UrlModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_urls.count();
}

QVariant UrlModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_urls.count())
        return {};

    switch (role) {
        case UrlRole: return m_urls.at(index.row());
    }

    return {};
}

QHash<int, QByteArray> UrlModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[UrlRole] = "url";
    return roles;
}

void UrlModel::addUrl(const QString &url) {
    qInfo() << QStringLiteral("UrlModel, addUrl: %1, m_urls.size: %2").arg(url).arg(m_urls.size());
    beginInsertRows(QModelIndex(), m_urls.size(), m_urls.size());
    m_urls.emplace_back(url);
    endInsertRows();
}
