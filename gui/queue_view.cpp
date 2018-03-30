#include "queue_view.h"

#include <QStandardItemModel>

#include "work_queue.h"

QueueView::QueueView(QWidget* parent) : QTableView(parent) {
    model = new QStandardItemModel(0, 1, this);
    model->setHeaderData(0, Qt::Horizontal, "File");
}

void QueueView::update_queue_view() {
    auto queue = queue_copy();
    model->setRowCount(queue.size());
    for (int i = 0; i < queue.size(); ++i) {
        model->setData(model->index(i, 0), QString::fromStdString(queue[i].first));
    }
}