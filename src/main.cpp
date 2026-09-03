#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "ui/logger/logcontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext &context, const QString &msg) {
        LogController::instance().logger(type, context, msg);
    });

    qInfo("Application started successfully.");

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);

    engine.loadFromModule("WebCrawlerApp", "Main");

    return app.exec();
}
