#include "work_server_wrapper.h"

#include "config.h"

WorkServerWrapper::WorkServerWrapper(Config* config) : WorkServer(config), event_handler(new QWorkServer) {}

void WorkServerWrapper::job_served(const std::string& path, BOT bot) {
    event_handler->emit_job_served(path, bot);
}

void WorkServerWrapper::job_submitted(const std::string& path, BOT bot) {
    event_handler->emit_job_submitted(path, bot);
}

QWorkServer::QWorkServer(QObject* parent) : QObject(parent) {}

void QWorkServer::emit_job_submitted(const std::string & path, BOT bot) {
    emit job_submitted(QString::fromStdString(path),
                    QString::fromStdString(string_from_bot(bot)));
}

void QWorkServer::emit_job_served(const std::string & path, BOT bot) {
    emit job_served(QString::fromStdString(path),
                    QString::fromStdString(string_from_bot(bot)));
}
