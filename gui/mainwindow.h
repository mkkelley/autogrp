#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QAbstractItemModel>

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

private slots:
    void on_pushButton_released();
    void downloader_finished();

private:
    void setup_model();
    void load_sgfs();

    Ui::MainWindow *ui;
    Config* config;
    QThread worker_thread;
    QAbstractItemModel* model;
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
