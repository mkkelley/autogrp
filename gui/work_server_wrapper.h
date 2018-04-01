#pragma once

#include "work_server.h"
#include <QObject>

class QWorkServer : public QObject {
    Q_OBJECT

public:
    QWorkServer(QObject* parent = nullptr);
    void emit_job_submitted(const std::string& path, BOT bot);
    void emit_job_served(const std::string& path, BOT bot);
signals:
    void job_submitted(QString job, QString bot);
    void job_served(QString job, QString bot);

};


class WorkServerWrapper : public WorkServer {
public:
    WorkServerWrapper(Config* config);

    QWorkServer* event_handler;
public:
    virtual void job_served(const std::string& path, BOT bot);
    virtual void job_submitted(const std::string& path, BOT bot);
};

