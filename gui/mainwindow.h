#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QAbstractItemModel>
#include "sgf_table_model.h"
#include "work_server_wrapper.h"

namespace Ui {
class MainWindow;
}

class Config;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Config* config, QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void on_pushButton_released();
    void downloader_finished();

private:
    Ui::MainWindow *ui;
    Config* config;
    QThread worker_thread;
    SgfTableModel* model;
    WorkServerWrapper work_server_wrapper;
};

class Downloader : public QObject {
    Q_OBJECT
public:
    Downloader(Config* config);
public slots:
    void run_downloader();
signals:
    void downloader_finished();
private:
    Config* config;
};

#endif // MAINWINDOW_H
