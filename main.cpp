#include "crawlermanager.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qcoreapplication.h>
#include <qobject.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("WebCrawlerApp", "Main");

    CrawlerManager manager;
    QObject::connect(&manager, &CrawlerManager::finished, &app, [](){
        qDebug() << "Crawling finished";
        QCoreApplication::exit(0);
    });
    
    manager.start("https://luckyland.com.ua/");
    return app.exec();
}
