#pragma once

#include <QObject>
#include <QAbstractListModel>
#include <qabstractitemmodel.h>
#include <qhashfunctions.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qvariant.h>


class UrlModel: public QAbstractListModel {
    Q_OBJECT
public:
    // struct UrlEnty {
    //     // int id;
    //     QString url;
    // };

    enum UrlRoles {
        UrlRole = Qt::UserRole + 1
    };

    explicit UrlModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void addUrl(const QString &url);

private:
    QStringList m_urls;
};
