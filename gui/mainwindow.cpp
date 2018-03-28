#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <QStandardItemModel>

#include "sgf_table_model.h"
#include "logutils.h"
#include "downloader.h"
#include "work_queue.h"
#include "move.h"
#include "config.h"

MainWindow::MainWindow(Config* config, QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    config(config)
{
    ui->setupUi(this);
    Downloader* downloader = new Downloader(config);
    downloader->moveToThread(&worker_thread);
    connect(&worker_thread, &QThread::finished, downloader, &QObject::deleteLater);
    connect(ui->pushButton, &QPushButton::released, downloader, &Downloader::run_downloader);
    connect(downloader, &Downloader::downloader_finished, this, &MainWindow::downloader_finished);
    worker_thread.start();

    setup_model();

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->resizeColumnsToContents();
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

void MainWindow::setup_model() {
    model = new SgfTableModel(config, this);
}

Downloader::Downloader(Config *config) : config(config) { }

void Downloader::run_downloader() {
    std::cout << get_timestamp() << " Running downloader\n";
    std::vector<std::string> new_games = download_missing_games(config);
    std::vector<std::string> bot_strings;
    std::string botconfig = config->bots_to_use;
    boost::split(bot_strings, botconfig, boost::is_any_of(" \t"), boost::token_compress_on);
    std::vector<BOT> bots_to_use;
    for (const auto& bot_string : bot_strings) {
        bots_to_use.push_back(bot_from_string(bot_string));
    }
    auto last = std::unique(bots_to_use.begin(), bots_to_use.end());
    bots_to_use.erase(last, bots_to_use.end());

    //constructing jobs outside so that we can use add to queue collection version instead of calling lock games*bots times
    std::vector<std::pair<std::string, BOT>> jobs;
    for (const auto& game : new_games) {
        for (BOT bot : bots_to_use) {
            jobs.emplace_back(game, bot);
        }
    }
    add_to_queue(jobs);
    emit downloader_finished();
}
