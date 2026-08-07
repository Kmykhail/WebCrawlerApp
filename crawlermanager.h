#ifndef CRAWLERMANAGER_H
#define CRAWLERMANAGER_H

#include <QNetworkAccessManager>
#include <QQueue>
#include <QRunnable>
#include <QTimer>
#include <qobjectdefs.h>
#include <qtmetamacros.h>

/*
    1. initialized
    2. contains queue and visitesUrls containers
    3. Need trigger when networdAccessManager started to pop elements from the queue
    4. Load hmtl
    5. Create QRunnable and add it to the global pool
    6. Receive data from the QRunnable worker by signal slot
*/

class CrawlerManager : public QObject
#include <QObject>
{
    Q_OBJECT
public:
    explicit CrawlerManager(QObject *parent = nullptr);
    void start(const QString &url);

public slots:
    void onUrlsParsed(const QSet<QString> &urls);

signals:
    void finished();

private:
    void loadHtml(const QString &url);
    void processQueue();

private:
    QNetworkAccessManager *m_networkAccessManager{nullptr};
    QQueue<QString> m_urlQueue;
    QSet<QString> m_visitedUrls;
    int m_depth{0};
    int m_activeDownloads{0};
    QString m_header{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"};
};


#endif // CRAWLERMANAGER_H
