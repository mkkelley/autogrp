
#include <QApplication>

#include "mainwindow.h"
#include "INIReader.h"
#include "config.h"

int main(int argc, char** argv) {
    QApplication qapp(argc, argv);
    INIReader config_reader("config.ini");
    Config config(&config_reader);
    MainWindow main_window(&config);
    main_window.show();
    return qapp.exec();
}
