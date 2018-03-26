
#include <QApplication>

#include "mainwindow.h"
#include "INIReader.h"

int main(int argc, char** argv) {
    QApplication qapp(argc, argv);
    INIReader config("config.ini");
    MainWindow main_window(&config);
    main_window.show();
    return qapp.exec();
}
