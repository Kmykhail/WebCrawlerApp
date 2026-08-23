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
#include <QTime>

#include "UrlData.h"

class UrlModel: public QAbstractListModel {
    Q_OBJECT

public:
    enum UrlRoles {
        UrlRole = Qt::UserRole + 1,
        TimeRole,
        StatusRole,
        DepthRole,
    };

    explicit UrlModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void onUrlsDiscovered(const QList<UrlData> &batch);

private:
    QList<UrlData> m_urlData;
};
