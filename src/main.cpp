#include <cstdio>

#include <QApplication>

#include <windows/mainwindow.hpp>

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    QApplication app(argc, argv);
    Windows::MainWindow window;
    window.show();

    return app.exec();
}