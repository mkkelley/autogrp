#include "progress_view.h"

const char* headers[] = {
    "File",
    "Bot"
};

ProgressView::ProgressView(QWidget* parent) : QTableView(parent) {
    model = new QStandardItemModel(0, 2, this);
    for (int i = 0; i < sizeof(headers) / sizeof(char*); ++i) {
        model->setHeaderData(i, Qt::Horizontal, headers[i]);
    }

    setModel(model);
}

void ProgressView::job_submitted(QString job, QString bot) {
    auto it = std::remove_if(jobs_in_progress.begin(), jobs_in_progress.end(),
                             [&job, &bot](auto& pair) {return pair.first == job && pair.second == bot; });
    jobs_in_progress.erase(it, jobs_in_progress.end());
    update_view();
}

void ProgressView::job_served(QString job, QString bot) {
    jobs_in_progress.emplace_back(job, bot);
    update_view();
}

void ProgressView::update_view() {
    model->setRowCount(jobs_in_progress.size());
    for (int i = 0; i < jobs_in_progress.size(); ++i) {
        model->setData(model->index(i, 0), jobs_in_progress[i].first);
        model->setData(model->index(i, 1), jobs_in_progress[i].second);
    }
}
