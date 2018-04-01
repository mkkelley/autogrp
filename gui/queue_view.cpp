#include "queue_view.h"

#include "work_queue.h"

QueueView::QueueView(QWidget* parent) : QTableView(parent) {
    model = new QStandardItemModel(0, 1, this);
    model->setHeaderData(0, Qt::Horizontal, "File");
    setModel(model);
    connect(this, &QueueView::job_requested, this, &QueueView::update_queue_view);
    update_queue_view();
}

void QueueView::submit_job(QString file, QString bot) {
    add_to_queue(file.toStdString(), bot_from_string(bot.toStdString()));
    emit job_requested(file, bot);
}

void QueueView::update_queue_view() {
    auto queue = queue_copy();
    model->setRowCount(queue.size());
    for (int i = 0; i < queue.size(); ++i) {
        model->setData(model->index(i, 0), QString::fromStdString(queue[i].first));
    }
}