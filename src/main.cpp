#include <cstdio>

#include <QApplication>
#include <QDebug>
#include <QEvent>
#include <qschematic/scene.hpp>
#include <private/qobject_p.h>

#include <windows/mainwindow.hpp>

//#define DEBUG_QT_EVENTS 1

#ifdef DEBUG_QT_EVENTS
class EventLogger : public QObject {
public:
    bool eventFilter(QObject *obj, QEvent *event) override {
        auto scenePtr = dynamic_cast<QSchematic::Scene *>(obj);
        if(scenePtr != nullptr) {
            qDebug() << "Objeto(Scene):" << obj << ", Tipo de evento:" << event->type();
        } else {
            qDebug() << "Objeto:" << obj << ", Tipo de evento:" << event->type();
        }

        if (event->type() == QEvent::MetaCall) {
            // MetaCallEvent es interno, así que necesitas un cast “crudo”
            QMetaCallEvent* callEvent = static_cast<QMetaCallEvent*>(event);
            if (callEvent) {
                qDebug() << "MetaCall para objeto:" << obj;
                qDebug() << "Signal Id:" << callEvent->signalId();
                // callEvent->method() no es accesible directamente, es privado
                // Pero podemos imprimir puntero de receiver y sender
                const QObject* sender = callEvent->sender();
                qDebug() << "Sender:" << sender;
            }
    }
        return QObject::eventFilter(obj, event); // retorna false para permitir que el evento continúe
    }
};
#endif

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    QApplication app(argc, argv);
    Windows::MainWindow window;
    window.show();

#ifdef DEBUG_QT_EVENTS
    EventLogger logger;
    app.installEventFilter(&logger);
#endif

    return app.exec();
}