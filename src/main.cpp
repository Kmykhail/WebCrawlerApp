#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qcoreapplication.h>
#include <qobject.h>
#include <qqml.h>

#include "crawlercontroller.h"
#include "UrlModel.h"

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

    qmlRegisterType<CrawlerController>("Crawler", 1, 0, "CrawlerController");
    qmlRegisterUncreatableType<UrlModel>("Crawler", 1, 0, "UrlModel", "Access to enums only");
    engine.loadFromModule("WebCrawlerApp", "Main");

    return app.exec();
}
