#include <QScreen>
#include <QGuiApplication>
#include <QPixmap>
#include <QPainter>
#include <QDateTime>
#include <QTest>

namespace Tests {

void saveScreenshot(const QString &stepName, QWidget *widget = nullptr) {
    if (!widget) return;

    QDir().mkpath("screenshots");

    const QString appName = QString(QTest::currentAppName()).split("/").last();
    const QString testFunc = QTest::currentTestFunction();

    QString autoName = QString("%1-%2-%3")
            .arg(appName)
            .arg(testFunc)
            .arg(stepName);

    QPixmap pix(widget->size());
    pix.fill(Qt::transparent);

    QPainter painter(&pix);
    widget->render(&painter);
    painter.end();

    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing, true);

    QFont font = p.font();
    font.setPointSize(22);
    font.setBold(true);
    p.setFont(font);

    QString watermark = QString("%1  -  %2")
        .arg(autoName)
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    QRect rect = pix.rect().adjusted(20, 20, -20, -20);

    p.setPen(Qt::white);
    p.setBrush(Qt::NoBrush);

    QColor bg(0, 0, 0, 128);
    p.fillRect(QRect(20, pix.height() - 70, pix.width() - 40, 50), bg);
    p.drawText(rect, Qt::AlignBottom | Qt::AlignLeft, watermark);
    p.end();

    QString filename = QString("screenshots/%1.png").arg(autoName);
    pix.save(filename, "PNG");
}

}