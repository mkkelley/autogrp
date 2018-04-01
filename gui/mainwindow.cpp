#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <QTimer>

#include "logutils.h"
#include "downloader.h"
#include "work_queue.h"
#include "move.h"
#include "config.h"
#include "analyze_sgf_delegate.h"

MainWindow::MainWindow(Config* config, QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    config(config),
    model(new SgfTableModel(config, this)),
    work_server_wrapper(config)
{
    ui->setupUi(this);
    Downloader* downloader = new Downloader(config);
    downloader->moveToThread(&worker_thread);
    connect(&worker_thread, &QThread::finished, downloader, &QObject::deleteLater);
    connect(ui->pushButton, &QPushButton::released, downloader, &Downloader::run_downloader);
    connect(downloader, &Downloader::downloader_finished, this, &MainWindow::downloader_finished);
    worker_thread.start();

    auto* delegate = new AnalyzeSgfDelegate(config, this);
    ui->tableView->setItemDelegate(delegate);

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->resizeColumnsToContents();

    connect(delegate, &AnalyzeSgfDelegate::analysis_requested, ui->queueView, &QueueView::submit_job);
    connect(downloader, &Downloader::downloader_finished, model, &SgfTableModel::update_table);

    connect(work_server_wrapper.event_handler, &QWorkServer::job_served,
            ui->queueView, &QueueView::update_queue_view);
    work_server_wrapper.start();

    connect(work_server_wrapper.event_handler, &QWorkServer::job_served,
            ui->progressView, &ProgressView::job_served);
    connect(work_server_wrapper.event_handler, &QWorkServer::job_submitted,
            ui->progressView, &ProgressView::job_submitted);
}

MainWindow::~MainWindow()
{
    worker_thread.quit();
    worker_thread.wait();
    delete ui;
}

void MainWindow::on_pushButton_released()
{
    ui->pushButton->setText("Downloader Running...");
    ui->pushButton->setEnabled(false);
}

void MainWindow::downloader_finished() {
    ui->pushButton->setText("Run Downloader Now");
    ui->pushButton->setEnabled(true);
}

Downloader::Downloader(Config *config) : config(config) { }

void Downloader::run_downloader() {
    std::cout << get_timestamp() << " Running downloader\n";
    std::vector<std::string> new_games = download_missing_games(config);

    emit downloader_finished();
}
