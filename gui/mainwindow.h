#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>

namespace Ui {
class MainWindow;
}

class INIReader;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(INIReader* config, QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_released();
    void downloader_finished();

private:
    Ui::MainWindow *ui;
    INIReader* config;
    QThread worker_thread;
};

class Downloader : public QObject {
    Q_OBJECT
public:
    Downloader(INIReader* config);
public slots:
    void run_downloader();
signals:
    void downloader_finished();
private:
    INIReader* config;
};

#endif // MAINWINDOW_H
