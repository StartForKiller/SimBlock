#include <cstdio>

#include <QApplication>

#include <mainwindow.hpp>

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    QApplication app(argc, argv);
    MainWindow window;
    window.show();

    return app.exec();
}