#include "orangeqpc.h"
#include <QApplication>
#include <QFile>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);



    // Загрузка стилей в стиле референса
    QFile styleFile("style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet);
    }

    OrangeQPC w;
    w.show();
    return a.exec();
}
