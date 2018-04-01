#pragma once

#include <QTableView>
#include <QStandardItemModel>

class ProgressView : public QTableView {
    Q_OBJECT
public:
    ProgressView(QWidget* parent = nullptr);

public slots:
    void job_submitted(QString job, QString bot);
    void job_served(QString job, QString bot);

private:
    void update_view();

    QStandardItemModel* model;
    std::vector<std::pair<QString, QString>> jobs_in_progress;
};