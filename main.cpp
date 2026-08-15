#include "crawlermanager.h"
#include "model/UrlModel.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qcoreapplication.h>
#include <qobject.h>
#include <qqml.h>

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

    qmlRegisterType<CrawlerManager>("Crawler", 1, 0, "CrawlerManager");
    qmlRegisterType<UrlModel>("Crawler", 1, 0, "UrlModel");
    engine.loadFromModule("WebCrawlerApp", "Main");

    return app.exec();
}
