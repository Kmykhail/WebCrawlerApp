#pragma once

#include <QObject>
#include <QRunnable>
#include <QUrl>
#include <QRegularExpression>
#include <qtclasshelpermacros.h>

class Worker : public QObject, public QRunnable {
    Q_OBJECT
    Q_DISABLE_COPY(Worker);
public:
    Worker(const QString& url, const QByteArray &html, QObject *parent = nullptr);
    void run() override;

signals:
    void urlParsed(const QSet<QString> &urls);

private:
    const QUrl m_qUrl;
    const QByteArray m_html;
    const QRegularExpression m_regex{"href=\"(https?://[^\"]+)\""};
};
