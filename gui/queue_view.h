#pragma once

#include <QTableView>
#include <QStandardItemModel>

class QueueView : public QTableView {
    Q_OBJECT

    QueueView(QWidget* parent = nullptr);

public slots:
    void update_queue_view();
private:
    QStandardItemModel* model;
};