#ifndef CRAWLERMANAGER_H
#define CRAWLERMANAGER_H

#include <QNetworkAccessManager>
#include <QQueue>
#include <QRunnable>
#include <QTimer>
#include <QThreadPool>
#include <QNetworkReply>
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
    Q_PROPERTY(bool running READ isRunning NOTIFY runningChanged)
public:
    explicit CrawlerManager(QObject *parent = nullptr);
    ~CrawlerManager();
    Q_INVOKABLE void start(const QString &url);
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    bool isRunning() const;

public slots:
    void onUrlsParsed(const QSet<QString> &urls);

signals:
    void finished();
    void runningChanged();
    void newUrlFound(const QString &url);

private:
    void loadHtml(const QString &url);
    void processQueue();
    void clearThreadPool();
    enum class ControlState {
        RUN, PAUSE, STOP
    };
    using enum ControlState;

private:
    int m_depth{0};
    int m_activeDownloads{0};
    ControlState m_controlState{STOP};

    QThreadPool m_threadPool;
    QQueue<QString> m_urlQueue;
    QSet<QString> m_visitedUrls;
    QSet<QNetworkReply*> m_activeReplies;

    QString m_header{"Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.3"};
    QNetworkAccessManager *m_networkAccessManager{nullptr};
};


#endif // CRAWLERMANAGER_H
