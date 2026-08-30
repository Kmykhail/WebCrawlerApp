#include "logcontroller.h"
#include "logmodel.h"

LogController::LogController(QObject *parent)
    : QObject{parent}
{}

LogController &LogController::instance()
{
    static LogController obj;
    return obj;
}

void LogController::logger(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray formattedMessage = qFormatLogMessage(type, context, msg).toUtf8();;
    fprintf(stderr, "%s\n", formattedMessage.constData());
    fflush(stderr);

    LogData data{QTime::currentTime(), type, msg};
    QMetaObject::invokeMethod(&LogModel::instance(), [data](){
        LogModel::instance().onAppendMessage(data);
    }, Qt::QueuedConnection);
}
