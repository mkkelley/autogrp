#pragma once

#include <QTableView>
#include <QStandardItemModel>

class QueueView : public QTableView {
    Q_OBJECT
public:
    QueueView(QWidget* parent = nullptr);
    void submit_job(QString file, QString bot);
signals:
    void job_requested(QString file, QString bot);
public slots:
    void update_queue_view();
private:
    QStandardItemModel* model;
};