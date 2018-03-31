
#include <QApplication>

#include "mainwindow.h"
#include "INIReader.h"
#include "config.h"
#include "work_server.h"

int main(int argc, char** argv) {
    std::string config_path = "config.ini";
    if (argc > 1) {
        config_path = argv[1];
    }
    QApplication qapp(argc, argv);
    INIReader config_reader(config_path);
    Config config(&config_reader);
    WorkServer server(&config);
    server.start();
    MainWindow main_window(&config);
    main_window.show();
    return qapp.exec();
}
