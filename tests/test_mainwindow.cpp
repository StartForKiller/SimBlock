#include <QtTest/QtTest>
#include <mainwindow.hpp>

class MainWindowTest : public QObject {
    Q_OBJECT

    private slots:
        void initTestCase() {
            qDebug("Called before everything else.");

            _window = new MainWindow();
            _window->show();
            QVERIFY(QTest::qWaitForWindowExposed(_window));
        }

        void cleanupTestCase() {
            delete _window;
        }

        void test_fail() {
            QCOMPARE("Hola", "Adios");
        }

    private:
        MainWindow *_window;
};

QTEST_MAIN(MainWindowTest)
#include "test_mainwindow.moc"