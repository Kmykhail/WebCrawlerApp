#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qcoreapplication.h>
#include <qobject.h>
#include <qqml.h>

#include "crawlercontroller.h"
#include "UrlModel.h"
#include "logmodel.h"
#include "logcontroller.h"

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

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        LogController::instance().logger(type, context, msg);
    });

    qmlRegisterType<CrawlerController>("Crawler", 1, 0, "CrawlerController");
    qmlRegisterUncreatableType<UrlModel>("Crawler", 1, 0, "UrlModel", "Access to enums only");
    qmlRegisterUncreatableType<CrawlerManager>("Crawler", 1, 0, "CrawlerManager", "Access to enums only");
    qmlRegisterSingletonInstance("Crawler", 1, 0, "LogModel", &LogModel::instance());
    engine.loadFromModule("WebCrawlerApp", "Main");

    return app.exec();
}
