#include "editor_main_window.h"
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow w;
    w.resize(1200, 800);
    w.show();

    return a.exec();
}