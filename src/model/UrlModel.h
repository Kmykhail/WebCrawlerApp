#pragma once

#include <QObject>
#include <QAbstractTableModel>
#include <qabstractitemmodel.h>
#include <qhashfunctions.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <QTime>

#include "UrlData.h"

class UrlModel: public QAbstractTableModel {
    Q_OBJECT

public:
    enum UrlRoles {
        UrlRole = Qt::UserRole + 1,
        TimeRole,
        StatusRole,
        DepthRole,
    };
    Q_ENUM(UrlRoles)

    explicit UrlModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void onUrlsDiscovered(const QList<UrlData> &batch);
    void onUpdateStatuses(const QHash<QString, quint16> &statusCodes);

private:
    qint32 findRowByKey(const QString &key) const;

private:
    QList<UrlData> m_urlData;
    QHash<quint64, quint16> m_urlHashIndex;
};
