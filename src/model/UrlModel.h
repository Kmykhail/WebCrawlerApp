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
        SizeRole,
        FetchedRole
    };
    Q_ENUM(UrlRoles)

    enum Columns {
        NumberColumn = 0,
        TimeColumn,
        StatusColumn,
        DepthColumn,
        UrlColumn,
        SizeColumn,
        FetchedColumn,
        ColumnCount
    };
    Q_ENUM(Columns)

    explicit UrlModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    void onUrlsDiscovered(const QList<UrlData> &rawBatch);
    void onUrlsFetched(const QList<UrlData> &fetchBatch);

private:
    qint32 findRowByKey(const QString &key) const;

private:
    QList<UrlData> m_urlData;
    QHash<quint64, qint32> m_urlHashIndex;
};
